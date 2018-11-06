//
// Created by Cavan Fyans (cavan@sensum.co) at Sensum on 25/10/18.
//

#include <ctime>
#include <iostream>
#include <curl/curl.h>

#ifndef LIVEAPI_DEMO_CPP_CURLDEBUG_H
#define LIVEAPI_DEMO_CPP_CURLDEBUG_H

class curlDebug {
 public:
  curlDebug();

  static void dump(const char *text,
                   FILE *stream, unsigned char *ptr, size_t size,
                   char nohex);

  static int my_trace(CURL *handle, curl_infotype type,
                      char *data, size_t size,
                      void *userp);

  struct data {
    char trace_ascii; /* 1 or 0 */
  };


};

#endif //LIVEAPI_DEMO_CPP_CURLDEBUG_H
