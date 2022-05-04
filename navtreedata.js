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
      [ "Setup", "md_docs_01_setup.html#autotoc_md3", null ]
    ] ],
    [ "Initialisation", "md_docs_02_initialisation.html", [
      [ "Import the library", "md_docs_02_initialisation.html#autotoc_md5", null ],
      [ "Creating a tracker", "md_docs_02_initialisation.html#autotoc_md6", [
        [ "<tt>storage</tt>", "md_docs_02_initialisation.html#autotoc_md7", null ],
        [ "<tt>emitter</tt>", "md_docs_02_initialisation.html#autotoc_md8", null ],
        [ "<tt>subject</tt>", "md_docs_02_initialisation.html#autotoc_md9", null ],
        [ "<tt>client_session</tt>", "md_docs_02_initialisation.html#autotoc_md10", null ],
        [ "<tt>platform</tt>", "md_docs_02_initialisation.html#autotoc_md11", null ],
        [ "<tt>app_id</tt>", "md_docs_02_initialisation.html#autotoc_md12", null ],
        [ "<tt>name_space</tt>", "md_docs_02_initialisation.html#autotoc_md13", null ],
        [ "<tt>use_base64</tt>", "md_docs_02_initialisation.html#autotoc_md14", null ],
        [ "<tt>desktop_context</tt>", "md_docs_02_initialisation.html#autotoc_md15", null ]
      ] ]
    ] ],
    [ "Adding extra data: the Subject class", "md_docs_03_adding_data.html", [
      [ "Set user ID with <tt>set_user_id</tt>", "md_docs_03_adding_data.html#autotoc_md17", null ],
      [ "Set screen resolution with <tt>set_screen_resolution</tt>", "md_docs_03_adding_data.html#autotoc_md18", null ],
      [ "Set viewport dimensions with <tt>set_viewport</tt>", "md_docs_03_adding_data.html#autotoc_md19", null ],
      [ "Set color depth with <tt>set_color_depth</tt>", "md_docs_03_adding_data.html#autotoc_md20", null ],
      [ "Set timezone with <tt>set_timezone</tt>", "md_docs_03_adding_data.html#autotoc_md21", null ],
      [ "Set the language with <tt>set_language</tt>", "md_docs_03_adding_data.html#autotoc_md22", null ],
      [ "Set custom user-agent with <tt>set_useragent</tt>", "md_docs_03_adding_data.html#autotoc_md23", null ],
      [ "Set user's IP address with <tt>set_ip_address</tt>", "md_docs_03_adding_data.html#autotoc_md24", null ]
    ] ],
    [ "Tracking specific events", "md_docs_04_tracking_events.html", [
      [ "Custom event context", "md_docs_04_tracking_events.html#autotoc_md26", null ],
      [ "Optional true-timestamp argument", "md_docs_04_tracking_events.html#autotoc_md27", null ],
      [ "Track SelfDescribing/Unstructured events with <tt>SelfDescribingEvent</tt>", "md_docs_04_tracking_events.html#autotoc_md28", null ],
      [ "Track screen views with <tt>ScreenViewEvent</tt>", "md_docs_04_tracking_events.html#autotoc_md29", null ],
      [ "Track structured events with <tt>StructuredEvent</tt>", "md_docs_04_tracking_events.html#autotoc_md30", null ],
      [ "Track timing events with <tt>TimingEvent</tt>", "md_docs_04_tracking_events.html#autotoc_md31", null ]
    ] ],
    [ "Emitters", "md_docs_05_emitters.html", [
      [ "Event store", "md_docs_05_emitters.html#autotoc_md33", null ],
      [ "Emitter request callback", "md_docs_05_emitters.html#autotoc_md34", null ],
      [ "Under the hood", "md_docs_05_emitters.html#autotoc_md35", null ],
      [ "HTTP request retry behavior", "md_docs_05_emitters.html#autotoc_md36", null ],
      [ "Request retry delay (back-off)", "md_docs_05_emitters.html#autotoc_md37", null ],
      [ "Manual flushing", "md_docs_05_emitters.html#autotoc_md38", null ],
      [ "Using a custom HTTP Client", "md_docs_05_emitters.html#autotoc_md39", null ]
    ] ],
    [ "Client Sessions", "md_docs_06_client_sessions.html", [
      [ "Session store", "md_docs_06_client_sessions.html#autotoc_md41", null ]
    ] ],
    [ "Upgrading to Newer Versions", "md_docs_07_upgrading.html", [
      [ "Upgrading to v0.3.0", "md_docs_07_upgrading.html#autotoc_md43", null ],
      [ "Upgrading to v0.2.0", "md_docs_07_upgrading.html#autotoc_md44", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"classsnowplow_1_1_subject.html#a9a01b3d93a98b656d2398f8292188480",
"namespacesnowplow.html#a7e5e750b3c366507f54aca9934cafa5e"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';