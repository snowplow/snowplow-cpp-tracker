/*
Copyright (c) 2023 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#if defined(__APPLE__)

#import "request_macos_interface.h"
#import "../../constants.hpp"
#import <Foundation/Foundation.h>

int snowplow::make_request(bool is_post, const string &url, const string &post_data) {
    NSString *nsUrl = [NSString stringWithUTF8String:url.c_str()];
    NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:nsUrl]];
    [urlRequest setValue:@"Snowplow C++ Tracker (macOS)" forHTTPHeaderField:@"User-Agent"];
    [urlRequest setValue:@"keep-alive" forHTTPHeaderField:@"Connection"];

    if (is_post) {
        NSData *data = [NSData dataWithBytes:post_data.data() length:post_data.length()];
        NSString *contentType = [NSString stringWithUTF8String:SNOWPLOW_POST_CONTENT_TYPE.c_str()];
        [urlRequest setValue:contentType forHTTPHeaderField:@"Content-Type"];
        [urlRequest setValue:[NSString stringWithFormat:@"%@", @(post_data.length()).stringValue] forHTTPHeaderField:@"Content-Length"];
        [urlRequest setHTTPMethod:@"POST"];
        [urlRequest setHTTPBody:data];
    } else {
        [urlRequest setHTTPMethod:@"GET"];
    }

    __block NSHTTPURLResponse *httpResponse = nil;
    __block NSError *connectionError = nil;
    dispatch_semaphore_t sem;

    sem = dispatch_semaphore_create(0);

    [[[NSURLSession sharedSession] dataTaskWithRequest:urlRequest
                                        completionHandler:^(NSData *data, NSURLResponse *urlResponse, NSError *error) {

        connectionError = error;
        httpResponse = (NSHTTPURLResponse*)urlResponse;

        dispatch_semaphore_signal(sem);
    }] resume];

    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);

    return int([httpResponse statusCode]);
}

#endif
