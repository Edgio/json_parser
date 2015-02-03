//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    whitebox_json_parser.cc
//: \details: Test driver to exercise the json_parser logic
//: \author:  Donnevan "Scott" Yeager
//: \date:    12/06/2014
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
//:

// define JSON_TRACE to do nothing, leave warnings/errors in place
#define JSON_TRACE(fmt, x...) do { } while(0)

#include "subbuffer.h"
#include "json_parser.h"
#include "wbtest.h"

#include <sys/time.h>
#include <string>
#include <vector>

bool check_string(json::value& val, subbuffer exp)
{
        return !val.is_unset() && val.is_string() && val.str().equals(exp);
}

bool check_numb(json::value& val, double exp)
{
        return !val.is_unset() && val.is_number() && val.numb() == exp;
}

bool check_bool(json::value& val, bool exp)
{
        return !val.is_unset() && val.is_bool() && val.bval() == exp;
}

bool check_array(json::value& val, size_t exp)
{
        return !val.is_unset() && val.is_array() && val.size() == exp;
}

bool check_object(json::value& val)
{
        return !val.is_unset() && val.is_object();
}

static void test_object(wbtester& t)
{
        subbuffer sb("{\"name\":\"Hal\",\"age\":23,\"aliases\":[\"Bob\",\"Joe\",\"Sam\"],\"wife\":{\"name\":\"wifey\"},\"old\":false,\"male\":true,\"female\":false},\"address\":null");
        json::root root(sb);
        t.REQUIRE(check_object(root));

        t.REQUIRE(false == root.exists("not_there"));
        t.REQUIRE(root["not_there"].is_unset());
        t.REQUIRE(false == root.exists("address"));
        t.REQUIRE(root["address"].is_unset());
        t.REQUIRE(check_string(root["name"], "Hal"));
        t.REQUIRE(root["name"].numb() == 0.0);
        t.REQUIRE(check_numb(root["age"], 23));
        t.REQUIRE(root["age"].str().empty());
        t.REQUIRE(check_array(root["aliases"], 3));
        t.REQUIRE(root["aliases"].raw_subbuffer().equals("[\"Bob\",\"Joe\",\"Sam\"]"));
        t.REQUIRE(check_object(root["wife"]));
        t.REQUIRE(root["wife"].raw_subbuffer().equals("{\"name\":\"wifey\"}"));
        t.REQUIRE(check_bool(root["old"], false));
        t.REQUIRE(check_bool(root["male"], true));
        t.REQUIRE(root["male"].str().equals("true"));
        t.REQUIRE(root["male"].numb() == 1.0);
        t.REQUIRE(check_bool(root["female"], false));
        t.REQUIRE(root["female"].str().equals("false"));
        t.REQUIRE(root["female"].numb() == 0.0);
}

static void test_object2(wbtester& t)
{
        const char* json_text = "\
[\
  {\
     \"key\"     : \"cache-purge\",	    \
     \"val\"\
:[\
   {\"uri\":\"abc\"}\
\
,\
\
   {\"uri\":\"123\"}\
\
\
	\
]\
,\
 \"expires\":\"9999999999\"\
  }\
]";

        json::root root(json_text);

        t.REQUIRE(check_array(root, 1));

        json::value& val = root[0];

        t.REQUIRE(check_object(val));

        t.REQUIRE(val[""].is_unset());
        t.REQUIRE(val["address"].is_unset());
        t.REQUIRE(check_string(val["key"], "cache-purge"));

        json::value& subval = val["val"];

        t.REQUIRE(check_object(subval[0]));
        t.REQUIRE(check_object(subval[1]));

        t.REQUIRE(check_string(subval[0]["uri"], "abc"));
        t.REQUIRE(check_string(subval[1]["uri"], "123"));

        t.REQUIRE(check_string(val["expires"], "9999999999"));
}

static void test_escaped(wbtester& t)
{
        json_object obj;
        obj.add("name", "bob");
        obj.add("desc", "bob is \"tall\"\n\"thin\"\n\"bald\"");

        std::string json_text;
        obj.to_string(json_text);

        json::root root(json_text);
        json::value& desc = root["desc"];
        t.REQUIRE(desc.str().equals("bob is \\\"tall\\\"\\\n\\\"thin\\\"\\\n\\\"bald\\\""));

        json_array arr;
        arr.add(1);
        arr.add("bob is \"tall\"\n\"thin\"\n\"bald\"");
        arr.add("hello");
        json_text.clear();
        arr.to_string(json_text);

        json::root aroot(json_text);
        t.REQUIRE(aroot[0].numb() == 1);
        t.REQUIRE(aroot[1].str().equals("bob is \\\"tall\\\"\\\n\\\"thin\\\"\\\n\\\"bald\\\""));
        t.REQUIRE(aroot[2].str().equals("hello"));
}

inline uint64_t get_microseconds()
{
        timeval tv;
        ::gettimeofday(&tv, 0);
        return static_cast <uint64_t>((static_cast <uint64_t>(tv.tv_sec) * 1000000)  + tv.tv_usec);
}

void run_perf_test(uint64_t perf_size)
{
        // build and array of perf_size OBJECTs, then parse it
        json_array arr;
        json_array arr2;
        json_object obj;
        char buff[100];

        for (uint64_t i = 0; i < perf_size; i++)
        {
                obj.clear();
                sprintf(buff, "srv%lu", i);
                obj.add(CONST_SUBBUF("host"), buff);
                obj.add(CONST_SUBBUF("index"), i);
                sprintf(buff, "A description for srv%lu", i);
                obj.add(CONST_SUBBUF("desc"), buff);
                arr2.clear();
                for (int a = 0; a < 100; a++)
                        arr2.add(a);
                obj.add(CONST_SUBBUF("numbs"), arr2);
                arr.add(obj);
        }

        std::string json_text;
        arr.to_string(json_text);

        uint64_t start = get_microseconds();

        json::root root(json_text);

        uint64_t parsed = get_microseconds();
        for (size_t s = 0; s < root.size(); s++)
        {
                json::value& val = root[s];
                if (val.is_unset())
                        fprintf(stderr, "Array element %zu is unset\n", s);
                else
                {
                        json::value& numbs = val[CONST_SUBBUF("numbs")];
                        if (numbs.is_unset())
                                fprintf(stderr, "Numbs is unset for array element %zu\n", s);
                        else if (numbs.size() != 100)
                                fprintf(stderr, "Numbs.size != 100 (%zu) for array element %zu\n", numbs.size(), s);
                }
        }

        uint64_t end = get_microseconds();
        uint64_t pars_ms = parsed - start;
        uint64_t check_ms = end - parsed;

        fprintf(stderr, "perf_test, perf_size: %lu, parse mic secs: %lu, check mic secs: %lu\n", perf_size, pars_ms, check_ms);
}

int main(int argc, char** argv)
{
        bool do_perf = false;
        uint64_t perf_size = 10000;
        for (int i = 1; i < argc; i++)
        {
                subbuffer arg(argv[i]);
                if (arg.equals(CONST_SUBBUF("--perf")))
                        do_perf = true;
                else if (arg.starts_with(CONST_SUBBUF("--perf-size=")))
                        perf_size = aton<uint64_t>(arg.after('='));
        }

        if (do_perf)
        {
                run_perf_test(perf_size);
                return 0;
        }

        wbtester t;

        t.ADD_TEST(test_object);
        t.ADD_TEST(test_object2);
        t.ADD_TEST(test_escaped);

        return t.run();
}
