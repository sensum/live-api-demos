# Sensum API Demo C++
Linux CLion Codebase for demo of the Sensum live API in C++

Contact - support@sensum.co

[sensum.co](https://www.sensum.co)
***
**Dependencies:**

AWS SDK - https://github.com/aws/aws-sdk-cpp

libcrypto - openSSL

libcurl - curl

boost - https://www.boost.org/

rapidjson - https://github.com/Tencent/rapidjson
***
**Setup:**

config.cpp needs to be populated with your credentials for Sensum API access etc:

* apikey
* region
* clientId
* userPoolId
* identityPoolId
* username
* password

In the config file you can also set the input file (csvFile) and output file names (outputCSV / outputTXT)
