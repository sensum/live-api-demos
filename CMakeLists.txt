cmake_minimum_required(VERSION 3.12)
project(liveAPI_demo_cpp)

set(CMAKE_CXX_STANDARD 14)

find_package(AWSSDK)

link_directories(/usr/local/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/Linux-x86_64
        ${CMAKE_CURRENT_SOURCE_DIR}/include/rapidjson)

add_executable(liveAPI_demo_cpp main.cpp Awssigv4.cc Awssigv4.h CurlDebug.cpp CurlDebug.h Config.cpp CognitoAuth.cpp)

target_link_libraries(liveAPI_demo_cpp
        aws-cpp-cognito-auth
        crypto
        aws-cpp-sdk-core
        aws-cpp-sdk-cognito-identity
        aws-cpp-sdk-cognito-idp
        curl
        )
