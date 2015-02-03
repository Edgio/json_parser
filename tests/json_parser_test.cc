//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    json_parser_test.cc
//: \details: Test json_parser against other parsers.
//: \author:  Donnevan "Scott Yeager
//: \date:    12/02/2014
//:
//:   Licensed under the Apache License, Version 2.0 (the "License");
//:   you may not use this file except in compliance with the License.
//:   You may obtain a copy of the License at
//:
//:       http://www.apache.org/licenses/LICENSE-2.0
//:
//:   Unless required by applicable law or agreed to in writing, software
//:   distributed under the License is distributed on an "AS IS" BASIS,
//:   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//:   See the License for the specific language governing permissions and
//:   limitations under the License.

#include "subbuffer.h"
#include "json_parser.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>


#include <sys/time.h>
#include <new>
#include <string>
#include <vector>

inline uint64_t get_microseconds()
{
        timeval tv;
        ::gettimeofday(&tv, 0);
        return static_cast <uint64_t>((static_cast <uint64_t>(tv.tv_sec) * 1000000)  + tv.tv_usec);
}

void build_perf_test_1_json(std::string& out, size_t objects, size_t numbers, size_t attributes)
{
        // build and array of perf_size OBJECTs, then parse it
        json_array arr;
        json_array arr2;
        json_object obj;
        char key[100];
        char val[100];

        for (uint64_t i = 0; i < objects; i++)
        {
                obj.clear();
                for (int a = 0; a < attributes; a++)
                {
                        sprintf(key, "%d_key_%lu", a, i);
                        sprintf(val, "val_%d_%lu, hello world", a, i);
                        obj.add(key, val);
                }
                arr2.clear();
                for (int a = 0; a < numbers; a++)
                        arr2.add(a);
                obj.add(CONST_SUBBUF("numbs"), arr2);
                arr.add(obj);
        }

        arr.to_string(out);
}

void run_rapidjson_test_1(const char* json_text, size_t objects, size_t numbers, size_t attributes)
{
        uint64_t start = get_microseconds();

        rapidjson::Document d;
        d.Parse<0>(json_text);

        uint64_t parsed = get_microseconds();

        char key[100];
        char val[100];

        for (size_t i = 0; i < d.Size(); i++)
        {
                const rapidjson::Value& obj = d[i];
                for (size_t a = 0; a < attributes; a++)
                {
                        sprintf(key, "%zu_key_%zu", a, i);
                        sprintf(val, "val_%zu_%zu, hello world", a, i);
                        const rapidjson::Value& v = obj[key];
                        if (strcmp(val, v.GetString()))
                                fprintf(stderr, "Incorrect attribute, key: %s, val: %s\n", key, v.GetString());
                }
                const rapidjson::Value& nums = obj["numbs"];
                if (nums.Size() != numbers)
                        fprintf(stderr, "Nums.Size != numbers (%u) for array element %zu\n", nums.Size(), i);
        }

        uint64_t end = get_microseconds();

        uint64_t parse_us = parsed - start;
        uint64_t check_us = end - parsed;

        printf("rapidjson DOM. Array of objects. Each object has N attributes including an array of numbers.\n"
               "\tJSON length: %zu bytes, objects: %zu, numbers: %zu, attributes: %zu, parse us: %lu, check us: %lu\n",
               strlen(json_text), objects, numbers, attributes, parse_us, check_us);
}

void run_json_parser_test_1(subbuffer json_text, size_t objects, size_t numbers, size_t attributes)
{
        uint64_t start = get_microseconds();

        json::root root(json_text);

        uint64_t parsed = get_microseconds();

        char key[100];
        char val[100];

        for (size_t i = 0; i < root.size(); i++)
        {
                json::value& obj = root[i];
                for (size_t a = 0; a < attributes; a++)
                {
                        sprintf(key, "%zu_key_%zu", a, i);
                        sprintf(val, "val_%zu_%zu, hello world", a, i);
                        const json::value& v = obj[key];
                        if (!v.str().equals(val))
                                fprintf(stderr, "Incorrect attribute, key: %s, val: %.*s\n", key, SUBBUF_FORMAT(v.str()));

                }
                if (obj.is_unset())
                        fprintf(stderr, "Array element %zu is unset\n", i);
                else
                {
                        json::value& numbs = obj[CONST_SUBBUF("numbs")];
                        if (numbs.is_unset())
                                fprintf(stderr, "Numbs is unset for array element %zu\n", i);
                        else if (numbs.size() != numbers)
                                fprintf(stderr, "Numbs.size != numbers (%zu) for array element %zu\n", numbs.size(), i);
                }
        }

        uint64_t end = get_microseconds();

        uint64_t parse_us = parsed - start;
        uint64_t check_us = end - parsed;

        printf("json_parser. Array of objects. Each object has N attributes including an array of numbers.\n"
               "\tJSON length: %zu bytes, objects: %zu, numbers: %zu, attributes: %zu, parse us: %lu, check us: %lu\n",
               json_text.length(), objects, numbers, attributes, parse_us, check_us);
}

void run_perf_test_1(subbuffer parser, size_t objects, size_t numbers, size_t attributes)
{
        std::string json_text;
        build_perf_test_1_json(json_text, objects, numbers, attributes);

        if (parser.equals("json_parser"))
        {
                run_json_parser_test_1(json_text.c_str(), objects, numbers, attributes);
        }
        else if (parser.equals("rapidjson"))
        {
                run_rapidjson_test_1(json_text.c_str(), objects, numbers, attributes);
        }
}

int main(int argc, char** argv)
{
        bool do_perf = false;
        size_t objects = 10000;
        size_t numbers = 100;
        size_t attributes = 10;
        subbuffer parser("json_parser");
        for (int i = 1; i < argc; i++)
        {
                subbuffer arg(argv[i]);
                if (arg.starts_with(CONST_SUBBUF("--objects=")))
                        objects = aton<size_t>(arg.after('='));
                else if (arg.starts_with(CONST_SUBBUF("--numbers=")))
                        numbers = aton<size_t>(arg.after('='));
                else if (arg.starts_with(CONST_SUBBUF("--attributes=")))
                        attributes = aton<size_t>(arg.after('='));
                else if (arg.starts_with(CONST_SUBBUF("--parser=")))
                        parser = arg.after('=');
        }

        run_perf_test_1(parser, objects, numbers, attributes);
}
