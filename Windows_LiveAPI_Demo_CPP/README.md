# Sensum API Demo C++
Windows Visual Studio codebase for demo of the Sensum live API in C++

Contact - support@sensum.co

[sensum.co](https://www.sensum.co)
***
**Dependencies:**

AWS SDK - https://github.com/aws/aws-sdk-cpp

libcrypto - openSSL (Win64 version 1.1.1) - http://slproweb.com/products/Win32OpenSSL.html

libcurl - curl (Download the pre-build curl (curldemo) for both x86 and x64, add the desired one to the project) - https://mariusbancila.ro/blog/2018/03/13/using-curl-library-from-c-on-windows/

boost (version 1.67.0) - https://www.boost.org/

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

**Visual Studio Setup:**

The demo has been tested on x64 machine running Visual Studio Community 2017 version 15.8.8: After loading the project from the solution add paths to the aditional include directories and Linker input directories would need to be changed according to the installation of these dependencies.

* Under "Project -> Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions" add these:
  - _CRT_SECURE_NO_WARNINGS
  - CURL_STATICLIB
  - USE_WINDOWS_DLL_SEMANTICS
  - ENABLE_WINDOWS_CLIENT
  - USE_IMPORT_EXPORT

* Under "Project -> Properties -> C/C++ -> General -> Additional Include Directories" add the paths, for example:
  - C:\Users\sensum\Windows_LiveAPI_Demo_CPP\include
  - C:\Program Files\aws-cpp-sdk-all\include
  - C:\Program Files\OpenSSL-Win64\include
  - C:\Users\sensum\Downloads\curldemo\curl\include
  - C:\Program Files\boost_1_67_0

* Under "Project -> Properties -> C/C++ -> Precompiled Headers -> Precompiled Header" select:
  - Not Using Precompiled Headers

* Under "Project -> Properties -> Linker -> General -> Additional Library Directories" add the paths, for example:
  - C:\Users\sensum\Downloads\curldemo\curl\build\lib\x64
  - C:\Program Files\OpenSSL-Win64\lib

* Under "Project -> Properties -> Linker -> Input -> Additional Dependencies" add these:
  - ws2_32.lib
  - crypt32.lib
  - libcrypto.lib
  - libcurl.lib
  - libcurld.lib
  - Wldap32.lib
