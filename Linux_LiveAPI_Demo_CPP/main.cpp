//
// Created by Cavan Fyans (cavan@sensum.co) at Sensum on 25/10/18.
//

#include <iostream>
#include <ctime>
#include "Awssigv4.h"
#include "aws/core/Aws.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include <curl/curl.h>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <thread>
#include "CurlDebug.h"
#include "Config.cpp"
#include "CognitoAuth.cpp"

#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"


std::string getISOTime() {
  time_t now;
  time(&now);
  char buf[sizeof "2011-10-08T07:07:09Z"];
  strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
  // this will work too, if your compiler doesn't support %F or %T:
  //strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  return buf;
}

int awsGetAuthHeader(Config &config, bool debug= false) {
  aws_sigv4::Signature signature(
      config.service,
      config.host,
      config.region,
      config.secretKey,
      config.accessKey
  );

  std::string method = config.method;
  std::string canonical_uri = config.canonical_uri;
  std::string queryString = "";
  std::map<std::string, std::vector<std::string> > canonical_header_map;
  std::string payload = config.payload;

  canonical_header_map["host"].push_back(config.host);
  canonical_header_map["accept"].push_back("application/json");
  canonical_header_map["content-type"].push_back("application/json");
  canonical_header_map["x-amz-date"].push_back(getISOTime());
  canonical_header_map["x-amz-security-token"].push_back(config.accessToken);
  canonical_header_map["x-api-key"].push_back(config.apiKey);

  std::string canonicalRequest = signature.createCanonicalRequest(method, canonical_uri, queryString, canonical_header_map, payload);
  std::string signedString = signature.createStringToSign(canonicalRequest);
  std::string calculatedSig = signature.createSignature(signedString);
  std::string authHeader = signature.createAuthorizationHeader(calculatedSig);

  if(debug) {
    std::cout << "canonicalRequest: " << "\n" <<canonicalRequest << "\n ----- \n";
    std::cout << signedString << "\n ----- \n";
    std::cout << calculatedSig << "\n ----- \n";
    std::cout << authHeader << "\n ----- \n";
  }

  config.setAuthHeader(authHeader);
  return 0;
}

size_t curlResponseHandler(void *contents, size_t size, size_t nmemb, std::string *s)
{
  size_t newLength = size*nmemb;
  size_t oldLength = s->size();
  try
  {
    s->resize(oldLength + newLength);
  }
  catch(std::bad_alloc &e)
  {
    //handle memory problem
    return 0;
  }

  std::copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
  return size*nmemb;
}

std::string callAPI(Config &config, bool debug = false) {
  CURL *curl;
  CURLcode res;
  struct curlDebug::data curlConfig;
  std::string response;

  if(debug) {
    curlConfig.trace_ascii = 1; //enable ascii tracing
  }

  //In windows, this will init the winsock stuff
  curl_global_init(CURL_GLOBAL_ALL);

  //get curl handle
  curl = curl_easy_init();
  if(curl) {
    if(debug) {
      curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curlDebug::my_trace);
      curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &curlConfig);
    }

    //set URL
    std::string reqUrl = "https://" + config.host + config.canonical_uri;
    curl_easy_setopt(curl, CURLOPT_URL, reqUrl.c_str());
    //set POST request
    curl_easy_setopt(curl, CURLOPT_POST, true);
    //set POST data:
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, config.payload.c_str());

    //compile header:
    struct curl_slist *chunk = NULL;
    //Add a custom headers
    std::string authHeader = "Authorization: " + config.authHeader;
    std::string token = "x-amz-security-token: " + config.accessToken;
    std::string date = "x-amz-date: " + getISOTime();
    std::string apiKey = "x-api-key: " + config.apiKey;


    chunk = curl_slist_append(chunk, authHeader.c_str());
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, apiKey.c_str());
    chunk = curl_slist_append(chunk, token.c_str());
    chunk = curl_slist_append(chunk, date.c_str());
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "api.sensum.co");

    //Add custom set of headers to request:
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    if(debug) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
    }

    //set callback handler function & output string for response:
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlResponseHandler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);


    //Send request:
    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    //cleanup:
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  return response;
}

int awsCognitoAuthenticate(Config &config) {
  try {
    //login to AWS Cognito and get credentials and tokens for signing requests:
    awsx::CognitoAuth auth(config.region, config.clientId);
    auto creds = auth.Authenticate(config.username, config.password, config.userPoolId, config.identityPoolId);

    if (!creds.GetAWSAccessKeyId().empty()) {
      std::__cxx11::string accessKey = creds.GetAWSAccessKeyId().c_str();
      std::__cxx11::string secretKey = creds.GetAWSSecretKey().c_str();
      std::__cxx11::string token = creds.GetSessionToken().c_str();

      config.setAccessKey(accessKey);
      config.setSecretKey(secretKey);
      config.setAccessToken(token);
    }
  }
  catch (const std::exception & x) {
    std::cerr << x.what() << std::endl;
  }
  catch (...) {
    std::cerr << "error" << std::endl;
  }

  return 0;
}

std::vector<std::vector<std::string> > getCSVData(std::string &fileName, std::string &delimiter) {
  std::ifstream file(fileName);

  std::vector<std::vector<std::string> > dataList;

  std::string line = "";
  //Iterate & split on delim:
  while (getline(file, line))
  {
    std::vector<std::string> vec;
    boost::algorithm::split(vec, line, boost::is_any_of(delimiter));
    dataList.push_back(vec);
  }
  file.close();

  return dataList;
}

std::string getUTCMillisString() {
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds >(
        std::chrono::_V2::system_clock::now().time_since_epoch()
    );
  std::__cxx11::string time = std::__cxx11::to_string(ms.count());
  return time;
}

int writeToFile(std::string &fileName, std::string &data) {
  std::ofstream file;
  file.open(fileName, std::ios::app);
  file << data;
  file.close();
  return 0;
}

int writeResponseToCSV(std::string fileName, std::string &response) {

	//blank response strings for empty response:
	std::string arousalDominant = "";
	std::string arousalVal = "";
	std::string arousalSecRelaxed = "";
	std::string arousalSecPassive = "";
	std::string arousalSecCalm = "";
	std::string arousalSecActivated = "";
	std::string arousalSecExcited = "";

	try {
		//Parse a JSON string into DOM:
		char * resp = new char[response.length() + 1];
		strcpy(resp, response.c_str());
		//Note: rapidjson has been used an example to parse the API response
		rapidjson::Document json;
		if (json.Parse(resp).HasParseError()) {
			//In case of parsing errors skip writing parsed json response to CSV
			//Due to parsing errors few asserstions fail in rapidjson which terminates the application
		} else {
			// write the valid parsed json response only to CSV
			arousalDominant = json["stats"]["arousal"]["dominant"].GetString();
			if (json["stats"]["arousal"]["value"].IsFloat()) arousalVal = std::to_string(json["stats"]["arousal"]["value"].GetFloat());
			if (json["stats"]["arousal"]["sectors"]["relaxed"].IsFloat()) arousalSecRelaxed = std::to_string(json["stats"]["arousal"]["sectors"]["relaxed"].GetFloat());
			if (json["stats"]["arousal"]["sectors"]["passive"].IsFloat()) arousalSecPassive = std::to_string(json["stats"]["arousal"]["sectors"]["passive"].GetFloat());
			if (json["stats"]["arousal"]["sectors"]["calm"].IsFloat()) arousalSecCalm = std::to_string(json["stats"]["arousal"]["sectors"]["calm"].GetFloat());
			if (json["stats"]["arousal"]["sectors"]["activated"].IsFloat()) arousalSecActivated = std::to_string(json["stats"]["arousal"]["sectors"]["activated"].GetFloat());
			if (json["stats"]["arousal"]["sectors"]["excited"].IsFloat()) arousalSecExcited = std::to_string(json["stats"]["arousal"]["sectors"]["excited"].GetFloat());

			//compile csv line:
			std::string writeData = arousalDominant + ","
				+ arousalVal + ","
				+ arousalSecRelaxed + ","
				+ arousalSecPassive + ","
				+ arousalSecCalm + ","
				+ arousalSecActivated + ","
				+ arousalSecExcited + ","
				+ "\n";

			writeToFile(fileName, writeData);
		}


	}
	catch (...) {
		std::cout << "Response JSON not valid, readable, or error in parsing" << std::endl;
	}

	return 0;
}

int writeResponseToText(std::string fileName, std::string response) {
  //write full JSON to file:
  response += "\n";
  writeToFile(fileName, response);

  return 0;
}

int writeCSVHeader(std::string &fileName) {
  //compile csv line:
  std::string writeData = "Arousal_Dominant_Label,Arousal,Arousal_Sector_Relaxed,Arousal_Sector_Passive,Arousal_Sector_Calm,Arousal_Sector_Activated,Arousal_Sector_Excited\n";

  std::ofstream file;
  file.open(fileName);
  file << writeData;
  file.close();

  return 0;
}

void processData(Config &config, bool debug) {
  //iterate through CSV data:
  for (const auto& inner : config.csvData) {
    //CSV col0 time, col1 value:
    //std::string time  = inner[0];

    //ignore CSV time value and use current time instead:
    std::string time = getUTCMillisString();
    //get data value:
    std::string value  = inner[1];

    //Build request data payload JSON:
    config.setPayload("{\"records\":{\"heartrate\":[{\"time\":" + time + ",\"value\":" + value + "}]}}");

    //Calculate request Auth header:
    awsGetAuthHeader(config, debug);

    //Perform API call, add 2nd param 'true' for debug:
    std::string response = callAPI(config, debug);

    writeResponseToText(config.outputTXT, response);
    writeResponseToCSV(config.outputCSV, response);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

int main() {
  bool debug = false;

  Config config;

  //read CSV to vector
  config.setCSVData(getCSVData(config.csvFile, config.csvDelimiter));

  //AWS SDK init:
  Aws::SDKOptions options;
  Aws::InitAPI(options);

  //Authenticate user with AWS Cognito & get tokens (tokens need to be refreshed after 60 minutes):
  awsCognitoAuthenticate(config);

  //init csv header:
  writeCSVHeader(config.outputCSV);

  processData(config, debug);

  //Cleanup
  Aws::ShutdownAPI(options);


//  while (true) {
//    std::string user_input;
//    getline(std::cin, user_input);
//    if (user_input == "s") {
//      break;
//    }
//  }

  return 0;
}
