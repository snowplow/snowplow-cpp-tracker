/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Snowplow C++ Tracker", "index.html", [
    [ "C++ Tracker", "index.html", null ],
    [ "Setup", "md_docs_01_setup.html", [
      [ "Tracker compatibility", "md_docs_01_setup.html#autotoc_md2", null ],
      [ "Installing in your project", "md_docs_01_setup.html#autotoc_md3", [
        [ "As an imported target in your CMake project", "md_docs_01_setup.html#autotoc_md4", null ],
        [ "As a subdirectory in your CMake project", "md_docs_01_setup.html#autotoc_md5", null ],
        [ "Copying files to your project", "md_docs_01_setup.html#autotoc_md6", null ],
        [ "Additional requirements under Linux", "md_docs_01_setup.html#autotoc_md7", null ]
      ] ]
    ] ],
    [ "Initialisation", "md_docs_02_initialisation.html", [
      [ "Import the library", "md_docs_02_initialisation.html#autotoc_md9", null ],
      [ "Option 1: Creating a tracker with default configuration using the \"Snowplow\" interface", "md_docs_02_initialisation.html#autotoc_md10", null ],
      [ "Option 2: Creating a tracker with custom configuration using the \"Snowplow\" interface", "md_docs_02_initialisation.html#autotoc_md11", [
        [ "Tracker configuration using \"TrackerConfiguration\"", "md_docs_02_initialisation.html#autotoc_md12", null ],
        [ "Network configuration using \"NetworkConfiguration\"", "md_docs_02_initialisation.html#autotoc_md13", null ],
        [ "Emitter configuration using \"EmitterConfiguration\"", "md_docs_02_initialisation.html#autotoc_md14", null ],
        [ "Session configuration using \"SessionConfiguration\"", "md_docs_02_initialisation.html#autotoc_md15", null ]
      ] ],
      [ "Option 3: Managing \"Tracker\", \"Emitter\", and \"ClientSession\" directly", "md_docs_02_initialisation.html#autotoc_md16", [
        [ "Storage", "md_docs_02_initialisation.html#autotoc_md17", null ],
        [ "Emitter", "md_docs_02_initialisation.html#autotoc_md18", null ],
        [ "Subject", "md_docs_02_initialisation.html#autotoc_md19", null ],
        [ "Client session", "md_docs_02_initialisation.html#autotoc_md20", null ],
        [ "Tracker", "md_docs_02_initialisation.html#autotoc_md21", null ]
      ] ]
    ] ],
    [ "Adding extra data: the Subject class", "md_docs_03_adding_data.html", [
      [ "Set user ID with \"set_user_id\"", "md_docs_03_adding_data.html#autotoc_md23", null ],
      [ "Set screen resolution with \"set_screen_resolution\"", "md_docs_03_adding_data.html#autotoc_md24", null ],
      [ "Set viewport dimensions with <tt>set_viewport</tt>[](#set-viewport-dimensions-with-set_viewport)", "md_docs_03_adding_data.html#autotoc_md25", null ],
      [ "Set color depth with <tt>set_color_depth</tt>[](#set-color-depth-with-set_color_depth)", "md_docs_03_adding_data.html#autotoc_md26", null ],
      [ "Set timezone with \"set_timezone\"", "md_docs_03_adding_data.html#autotoc_md27", null ],
      [ "Set the language with <tt>set_language</tt>[](#set-the-language-with-set_language)", "md_docs_03_adding_data.html#autotoc_md28", null ],
      [ "Set custom user-agent with \"set_useragent\"", "md_docs_03_adding_data.html#autotoc_md29", null ],
      [ "Set user's IP address with \"set_ip_address\"", "md_docs_03_adding_data.html#autotoc_md30", null ]
    ] ],
    [ "Tracking specific events", "md_docs_04_tracking_events.html", [
      [ "Custom event context", "md_docs_04_tracking_events.html#autotoc_md32", null ],
      [ "Optional true-timestamp argument", "md_docs_04_tracking_events.html#autotoc_md33", null ],
      [ "Track SelfDescribing/Unstructured events with \"SelfDescribingEvent\"", "md_docs_04_tracking_events.html#autotoc_md34", null ],
      [ "Track screen views with \"ScreenViewEvent\"", "md_docs_04_tracking_events.html#autotoc_md35", null ],
      [ "Track structured events with <tt>StructuredEvent</tt>[](#track-structured-events-with-trackstructevent)", "md_docs_04_tracking_events.html#autotoc_md36", null ],
      [ "Track timing events with \"TimingEvent\"", "md_docs_04_tracking_events.html#autotoc_md37", null ]
    ] ],
    [ "Emitters", "md_docs_05_emitters.html", [
      [ "Event store", "md_docs_05_emitters.html#autotoc_md39", null ],
      [ "Emitter request callback", "md_docs_05_emitters.html#autotoc_md40", null ],
      [ "HTTP request retry behavior", "md_docs_05_emitters.html#autotoc_md41", null ],
      [ "Request retry delay (back-off)", "md_docs_05_emitters.html#autotoc_md42", null ],
      [ "Manual flushing", "md_docs_05_emitters.html#autotoc_md43", null ],
      [ "Using a custom HTTP Client", "md_docs_05_emitters.html#autotoc_md44", null ]
    ] ],
    [ "Client Sessions", "md_docs_06_client_sessions.html", [
      [ "Session store", "md_docs_06_client_sessions.html#autotoc_md46", null ]
    ] ],
    [ "Upgrading to Newer Versions", "md_docs_07_upgrading.html", [
      [ "Upgrading to v2.0.0", "md_docs_07_upgrading.html#autotoc_md48", null ],
      [ "Upgrading to v1.0.0", "md_docs_07_upgrading.html#autotoc_md49", null ],
      [ "Upgrading to v0.3.0", "md_docs_07_upgrading.html#autotoc_md50", null ],
      [ "Upgrading to v0.2.0", "md_docs_07_upgrading.html#autotoc_md51", null ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"index.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';