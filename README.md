<!------------------------------------------------------------------------------
# Copyright (C) 2015 Verizon.  All Rights Reserved.
# All Rights Reserved
#
#   Author: Donnevan "Scott" yeager
#   Date:   02/03/2015
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
------------------------------------------------------------------------------>

json_parser
===========

Lightweight, header only DOM json_parser. Fairly fast and fairly light. Built to handle most JSON. Any encoded/escaped test fields in the JSON will still be encoded/escaped after they are parsed. It is up to the user to decide which fields need unencoding.

So far I have only compared json_parser's speed against rapidjson. For those comparisons I parsed 140MB of JSON which had 100K objects. Each object had 30 key/value pairs with string values. Each object also had an array of 50 numbers (doubles). The closest I came was 1.8 times slower than rapidjson. Each test run was a separate run of the process. After parsing the JSON, each test walked through the resuls checking the validity of the values.
    json_parser: 141533401 bytes, objects: 100000, numbers: 50, attributes: 30, parse us: 1465640, check us: 1750130     // -O2
    json_parser: 141533401 bytes, objects: 100000, numbers: 50, attributes: 30, parse us: 1374260, check us: 1701616     // -O3
    rapidjson:   141533401 bytes, objects: 100000, numbers: 50, attributes: 30, parse us: 819425,  check us: 1767048
    rapidjson:   141533401 bytes, objects: 100000, numbers: 50, attributes: 30, parse us: 811476,  check us: 1766306 

Example Usage: 
    // This example is fully coded in the examples dir.
    std::string json_buffer;
    // fill json_buffer with json data from somewhere (file, HTTP, TCP, etc)
    // Assuming that the JSON has an object with a temperature value which has key/values for the months, each month having an array of temps.
    // {"temperature":{"January":[1,2,3...4],...."March":[40,40,....46,47]}}
    ....
    json::root root(json_buffer);
    if (root.is_unset())
            // return failure
    json::value& temp = root["temperature"];
    if (temp.is_unset()) // return failure
    json::value& march = temp["March"];
    if (march.is_unset()) // return failure
    double march15 = march[15].numb()
    
The whitebox tests are the only files that need built. The rest of the files are header only implementations so just include them and use them.

Build the whitebox tests (out-of-tree suggested):

    mkdir -p build && cd build;
    cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
    make
    make test

Build the whitebox tests for profiling with gcov:

    mkdir -p build && cd build;
    cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
    make
    make test

Note: The whitebox_aton test takes a while because it is doing timings. There is a noticable difference in the time to run it between Release and Debug.


Helper Classes
--------------

They are packaged here as helper classes, however their uses potentially extend beyond just this application.

`subbuffer`: A const char* and length wrapped into a class with easy to understand methods added. Unless we specifically need a NULL terminated string, subbuffers are passed in all new code instead of `const char*` and `const string&`.

`subparser`: Parses `subbuffers` with a user supplied delimiter. The delimiter can be a `char`, `chargrp` or another `subbuffer`.

`aton`: Performs alphanumeric to numeric conversions on `subbuffers`.

`chargrp`: A set of ascii chars (0-255) used for comparisons. If you want to match on a char in a group of user specified chars in any order, this works.

`json_array` and `json_object`: Provides an easy way of building JSON output.
