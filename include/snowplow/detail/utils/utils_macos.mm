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

#import "utils_macos.h"

@implementation UtilsMacOS

string snowplow::get_os_version_objc() {
    return ([[UtilsMacOS getOSVersion] UTF8String]);
}

+ (NSString *) getOSVersion {
  SInt32 major;
  SInt32 minor;
  SInt32 patch;

#if defined(MAC_OS_X_VERSION_10_10) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_10
  NSProcessInfo *info = [NSProcessInfo processInfo];
  NSOperatingSystemVersion systemVersion = [info operatingSystemVersion];
  major = (int)systemVersion.majorVersion;
  minor = (int)systemVersion.minorVersion;
  patch = (int)systemVersion.patchVersion;
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  Gestalt(gestaltSystemVersionMajor, &major);
  Gestalt(gestaltSystemVersionMinor, &minor);
  Gestalt(gestaltSystemVersionBugFix, &patch);
#pragma clang diagnostic pop
#endif

  return [NSString stringWithFormat:@"%d.%d.%d", major, minor, patch];
}

@end

#endif
