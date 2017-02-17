//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    whitebox_subparser.cc
//: \details: Test driver to exercise the subparser logic
//: \author:  Donnevan "Scott" Yeager
//: \date:    09/22/2016
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

#include "subparser.h"
#include "wbtest.h"

#include <string>

static void test_empty(wbtester& t)
{
        subbuffer sb; // an empty subbuffer
        subparser prsr(sb, ',', subparser::SKIP_EMPTY);
        t.REQUIRE(prsr.empty());

        subbuffer sb2(",,,,,,"); // an empty subbuffer
        subparser prsr2(sb2, ',', subparser::SKIP_EMPTY);
        t.REQUIRE(prsr2.empty());
}

static void test_char_delim(wbtester& t)
{
        subbuffer sb("..marry.had....a.little..lamb");
        subparser prsr(sb, '.', subparser::SKIP_EMPTY);
        t.REQUIRE(!prsr.empty());
        t.REQUIRE(prsr.next().equals("marry"));
        t.REQUIRE(prsr.next().equals("had"));
        t.REQUIRE(prsr.next().equals("a"));
        t.REQUIRE(prsr.next().equals("little"));
        t.REQUIRE(prsr.next().equals("lamb"));
        t.REQUIRE(prsr.empty());

        subparser prsr2(sb, '.', subparser::RETURN_EMPTY);
        t.REQUIRE(!prsr2.empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("marry"));
        t.REQUIRE(prsr2.next().equals("had"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("a"));
        t.REQUIRE(prsr2.next().equals("little"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("lamb"));
        t.REQUIRE(prsr2.empty());
}

static void test_subbuffer_delim(wbtester& t)
{
        subbuffer sb("marryateatehadateaatelittleateateatelamb");
        subparser prsr(sb, "ate", subparser::SKIP_EMPTY);
        t.REQUIRE(!prsr.empty());
        t.REQUIRE(prsr.next().equals("marry"));
        t.REQUIRE(prsr.next().equals("had"));
        t.REQUIRE(prsr.next().equals("a"));
        t.REQUIRE(prsr.next().equals("little"));
        t.REQUIRE(prsr.next().equals("lamb"));
        t.REQUIRE(prsr.empty());

        subparser prsr2(sb, "ate", subparser::RETURN_EMPTY);
        t.REQUIRE(!prsr2.empty());
        t.REQUIRE(prsr2.next().equals("marry"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("had"));
        t.REQUIRE(prsr2.next().equals("a"));
        t.REQUIRE(prsr2.next().equals("little"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("lamb"));
        t.REQUIRE(prsr2.empty());
}

static void test_chargrp_delim(wbtester& t)
{
        subbuffer sb("marry. had .a# little#..#lamb");
        chargrp delims(" .#");
        subparser prsr(sb, delims, subparser::SKIP_EMPTY);
        t.REQUIRE(!prsr.empty());
        t.REQUIRE(prsr.next().equals("marry"));
        t.REQUIRE(prsr.next().equals("had"));
        t.REQUIRE(prsr.next().equals("a"));
        t.REQUIRE(prsr.next().equals("little"));
        t.REQUIRE(prsr.next().equals("lamb"));
        t.REQUIRE(prsr.empty());

        subparser prsr2(sb, delims, subparser::RETURN_EMPTY);
        t.REQUIRE(!prsr2.empty());
        t.REQUIRE(prsr2.next().equals("marry"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("had"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("a"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("little"));
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().empty());
        t.REQUIRE(prsr2.next().equals("lamb"));
        t.REQUIRE(prsr2.empty());
}

int main(int argc, char** argv)
{
        wbtester t;

        t.ADD_TEST(test_empty);
        t.ADD_TEST(test_char_delim);
        t.ADD_TEST(test_subbuffer_delim);
        t.ADD_TEST(test_chargrp_delim);

        return t.run();
}
