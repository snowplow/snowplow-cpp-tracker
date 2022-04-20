/*
Copyright (c) 2022 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#if defined(__APPLE__)
#include "http_client_apple.hpp"
#include "../constants.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>
#include <CFNetwork/CFHTTPStream.h>

using namespace snowplow;
using std::cerr;
using std::endl;
using std::lock_guard;

const string HttpClientApple::TRACKER_AGENT = string("Snowplow C++ Tracker (macOS)");

HttpRequestResult HttpClientApple::http_request(const RequestMethod method, CrackedUrl url, const string &query_string, const string &post_data, list<int> row_ids, bool oversize) {

  // Get final url
  string final_url = url.to_string();
  if (method == GET) {
    final_url += "?" + query_string;
  }

  // Create request
  CFStringRef cf_url_str = CFStringCreateWithBytes(kCFAllocatorDefault, (const unsigned char *)final_url.c_str(), final_url.length(), kCFStringEncodingUTF8, false);
  CFStringRef cf_content_type_str = CFStringCreateWithBytes(kCFAllocatorDefault, (const unsigned char *)SNOWPLOW_POST_CONTENT_TYPE.c_str(), SNOWPLOW_POST_CONTENT_TYPE.length(), kCFStringEncodingUTF8, false);
  CFStringRef cf_user_agent_str = CFStringCreateWithBytes(kCFAllocatorDefault, (const unsigned char *)HttpClientApple::TRACKER_AGENT.c_str(), HttpClientApple::TRACKER_AGENT.length(), kCFStringEncodingUTF8, false);

  CFURLRef cf_url = CFURLCreateWithString(kCFAllocatorDefault, cf_url_str, NULL);
  CFHTTPMessageRef cf_http_req;

  if (method == GET) {
    cf_http_req = CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("GET"), cf_url, kCFHTTPVersion1_1);
  } else {
    cf_http_req = CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("POST"), cf_url, kCFHTTPVersion1_1);
    CFDataRef cf_post_data = CFDataCreate(kCFAllocatorDefault, (const UInt8 *)post_data.data(), post_data.size());
    CFHTTPMessageSetBody(cf_http_req, cf_post_data);
    if (cf_post_data) {
      CFRelease(cf_post_data);
    }
    CFHTTPMessageSetHeaderFieldValue(cf_http_req, CFSTR("Content-Type"), cf_content_type_str);
  }
  CFHTTPMessageSetHeaderFieldValue(cf_http_req, CFSTR("User-Agent"), cf_user_agent_str);
  CFHTTPMessageSetHeaderFieldValue(cf_http_req, CFSTR("Connection"), CFSTR("keep-alive"));

  CFReadStreamRef cf_read_stream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, cf_http_req);
  CFMutableDataRef cf_data_resp = CFDataCreateMutable(kCFAllocatorDefault, 0);

  // Send request
  CFReadStreamOpen(cf_read_stream);
  CFIndex num_bytes_read;
  do {
    const int buff_size = 1024;
    UInt8 buff[buff_size];
    num_bytes_read = CFReadStreamRead(cf_read_stream, buff, buff_size);

    if (num_bytes_read > 0) {
      CFDataAppendBytes(cf_data_resp, buff, num_bytes_read);
    } else if (num_bytes_read < 0) {
      CFStreamError error = CFReadStreamGetError(cf_read_stream);
      cerr << error.error << endl;
    }
  } while (num_bytes_read > 0);

  // Process result
  CFHTTPMessageRef cf_http_resp = (CFHTTPMessageRef)CFReadStreamCopyProperty(cf_read_stream, kCFStreamPropertyHTTPResponseHeader);
  int cf_status_code = CFHTTPMessageGetResponseStatusCode(cf_http_resp);

  // Release resources
  CFReadStreamClose(cf_read_stream);
  CFRelease(cf_url_str);
  CFRelease(cf_content_type_str);
  CFRelease(cf_user_agent_str);
  CFRelease(cf_url);
  CFRelease(cf_http_req);
  CFRelease(cf_read_stream);
  CFRelease(cf_data_resp);
  CFRelease(cf_http_resp);

  return HttpRequestResult(0, cf_status_code, row_ids, oversize);
}

#endif
