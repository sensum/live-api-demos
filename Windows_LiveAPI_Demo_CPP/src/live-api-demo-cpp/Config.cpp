#include <string>
#include <vector>
#include <chrono>

class Config {
public:
	Config() {
		//append output files with timestamp:
		std::string time = getUTCMillisString();
		outputTXT += "_" + time + ".txt";
		outputCSV += "_" + time + ".csv";
	};

	std::string csvFile = "C:\\Users\\sensum\\VS_LiveAPI_Demo_CPP\\heartrate_sample.csv";
	std::string csvDelimiter = ",";
	std::vector<std::vector<std::string> > csvData;

	//filename timestamp & extensions appended in constructor:
	std::string outputCSV = "output";
	std::string outputTXT = "responses";

	std::string method = "POST";
	std::string canonical_uri = "/dev/events/";
	std::string host = "api.sensum.co";
	std::string apiKey = "";
	std::string service = "execute-api";
	std::string region = "";
	std::string secretKey;
	std::string accessKey;
	std::string accessToken;
	std::string authHeader;
	std::string clientId = "";
	std::string userPoolId = "";		// without region
	std::string identityPoolId = "";	// without region
	std::string username = "";
	std::string password = "";
	std::string payload;

	void setSecretKey(std::string _secretKey) {
		secretKey = _secretKey;
	}
	void setAccessKey(std::string _accessKey) {
		accessKey = _accessKey;
	}
	void setAccessToken(std::string _accessToken) {
		accessToken = _accessToken;
	}
	void setAuthHeader(std::string _authHeader) {
		authHeader = _authHeader;
	}
	void setPayload(std::string _payload) {
		payload = _payload;
	}
	void setCSVData(std::vector<std::vector<std::string> > _csvData) {
		csvData = _csvData;
	}

private:
	std::string getUTCMillisString() {
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			);
		std::string time = std::to_string(ms.count());
		return time;
	}

};