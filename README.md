# liveAPI_Demo_CPP
Codebase for demo of the Sensum live API in C++

Dependencies:
AWS SDK - https://github.com/aws/aws-sdk-cpp
libcrypto - openSSL
libcurl - curl

config.cpp needs to be populated with your credentials for Sensum API access etc:

apikey
region
clientId
userPoolId
identityPoolId
username
password

In the config file you can also set the input file (csvFile) and output file names (outputCSV / outputTXT)
