//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    whitebox_aton.cc
//: \details: Test driver for aton functionality
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

#include "aton.h"
#include "wbtest.h"

#include <algorithm>
#include <vector>

#include <sys/time.h>

static uint64_t get_microseconds()
{
        timeval tv;
        ::gettimeofday(&tv, 0);
        return static_cast <uint64_t>((static_cast <uint64_t>(tv.tv_sec) * 1000000)  + tv.tv_usec);
}

static void test_overflow(wbtester& t)
{
        subbuffer sb_u64_max("18446744073709551615");
        subbuffer sb_u64_over("18446744073709551617");

        subbuffer rem;
        t.REQUIRE(aton<uint64_t>(sb_u64_over, &rem) == std::numeric_limits<uint64_t>::max());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_OVERFLOW")));

        t.REQUIRE(aton<uint64_t>(sb_u64_max, &rem) == std::numeric_limits<uint64_t>::max());
        t.REQUIRE(rem.empty());
        t.REQUIRE(aton<uint32_t>(sb_u64_max, &rem) == std::numeric_limits<uint32_t>::max());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_OVERFLOW")));
        t.REQUIRE(aton<uint16_t>(sb_u64_max, &rem) == std::numeric_limits<uint16_t>::max());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_OVERFLOW")));
        t.REQUIRE(aton<uint8_t>(sb_u64_max, &rem) == std::numeric_limits<uint8_t>::max());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_OVERFLOW")));

        subbuffer sb_neg_one("-1");
        t.REQUIRE(aton<uint64_t>(sb_neg_one, &rem) == 0);
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
        t.REQUIRE(aton<uint32_t>(sb_neg_one, &rem) == 0);
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
        t.REQUIRE(aton<uint16_t>(sb_neg_one, &rem) == 0);
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
        t.REQUIRE(aton<uint8_t>(sb_neg_one, &rem) == 0);
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));

        subbuffer sb_neg_big("-18446744073709551617");
        t.REQUIRE(aton<int64_t>(sb_neg_big, &rem) == std::numeric_limits<int64_t>::min());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
        t.REQUIRE(aton<int32_t>(sb_neg_big, &rem) == std::numeric_limits<int32_t>::min());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
        t.REQUIRE(aton<int16_t>(sb_neg_big, &rem) == std::numeric_limits<int16_t>::min());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
        t.REQUIRE(aton<int8_t>(sb_neg_big, &rem) == std::numeric_limits<int8_t>::min());
        t.REQUIRE(rem.equals(CONST_SUBBUF("ATON_UNDERFLOW")));
}

static void test_simple(wbtester& t)
{
        subbuffer line("12345678901234567890");
        for (size_t i = 0; i < line.length(); i++)
        {
                t.REQUIRE(aton<uint8_t>(line.sub(i,1)) == (line.at(i) - '0'));
        }
        t.REQUIRE(aton<int8_t>("-125") == -125);
        t.REQUIRE(aton<int8_t>("125") == 125);
        t.REQUIRE(aton<int8_t>("+125") == 125);
        t.REQUIRE(aton<int16_t>("-32001") == -32001);
        t.REQUIRE(aton<int16_t>("+32001") == 32001);
        t.REQUIRE(aton<int16_t>("32001") == 32001);
        t.REQUIRE(aton<int32_t>("-2000000001") == -2000000001);
        t.REQUIRE(aton<int32_t>("+2000000001") == 2000000001);
        t.REQUIRE(aton<int32_t>("2000000001") == 2000000001);
        t.REQUIRE(aton<int64_t>("-922337203685477580") == -922337203685477580LL);
        t.REQUIRE(aton<int64_t>("+922337203685477580") == 922337203685477580LL);
        t.REQUIRE(aton<uint64_t>("+922337203685477580") == 922337203685477580LL);
        t.REQUIRE(aton<int64_t>("922337203685477580") == 922337203685477580LL);
        t.REQUIRE(aton<int64_t>("0") == 0LL);
        t.REQUIRE(aton<uint32_t>("000A", NULL, 16) == 10);
        t.REQUIRE(aton<int32_t>("000A", NULL, 16) == 10);
        subbuffer remainder;
        t.REQUIRE(aton<int32_t>("000AG", &remainder, 16) == 10);
        t.REQUIRE(remainder.equals('G'));
        t.REQUIRE(aton<uint32_t>("000AG", &remainder, 16) == 10); // test with unsigned
        t.REQUIRE(remainder.equals('G'));
        t.REQUIRE(aton<int64_t>(subbuffer()) == 0);
        t.REQUIRE(aton<uint64_t>(subbuffer()) == 0);

        t.REQUIRE(aton<int32_t>("3000000000", &remainder, 10) == std::numeric_limits<int32_t>::max());
        t.REQUIRE(remainder.equals("ATON_OVERFLOW"));
}

template<typename NUMB> bool test_hex()
{
        char buff[25];
        char buff2[25];
        uint32_t low = 1;
        uint32_t high = 10000000;
        for (uint32_t i = low; i <= high; i++)
        {
                ::sprintf(buff, "%04x", i);
                ::sprintf(buff2, "%04X", i);
                NUMB val = aton<NUMB>(buff, NULL, 16);
                NUMB val2 = aton<NUMB>(buff2, NULL, 16);
                if (val != (NUMB)i)
                {
                        ::printf("NUMB, buff: '%s', i: %d, val: %d\n", buff, i, int32_t(val));
                        return false;
                }
                if (val2 != (NUMB)i)
                {
                        ::printf("NUMB, buff: '%s', i: %d, val: %d\n", buff2, i, int32_t(val2));
                        return false;
                }
        }
        return true;
}

template<typename NUMB> bool test_aton()
{
        char buff[25];
        int32_t low = std::max((int32_t)-5000000, (int32_t)std::numeric_limits<NUMB>::min());
        int32_t high = std::min((int32_t)10000000, (int32_t)std::numeric_limits<NUMB>::max());
        if (high < 0) high = 10000000;
        for (int32_t i = low; i <= high; i++)
        {
                ::sprintf(buff, "%d", i);
                NUMB val = aton<NUMB>(buff);
                if (val != (NUMB)i)
                {
                        ::printf("NUMB, buff: '%s', i: %d, val: %d\n", buff, i, int32_t(val));
                        return false;
                }
        }
        return true;
}

static void test_extensive(wbtester& t)
{
        t.REQUIRE(test_aton<int8_t>() == true);
        t.REQUIRE(test_aton<int16_t>() == true);
        t.REQUIRE(test_aton<int32_t>() == true);
        t.REQUIRE(test_aton<int64_t>() == true);

        t.REQUIRE(test_aton<uint8_t>() == true);
        t.REQUIRE(test_aton<uint16_t>() == true);
        t.REQUIRE(test_aton<uint32_t>() == true);
        t.REQUIRE(test_aton<uint64_t>() == true);

        t.REQUIRE(test_hex<uint32_t>() == true);
        t.REQUIRE(test_hex<uint64_t>() == true);
}

static void test_floating_point(wbtester& t)
{
        char buff[50];
        char buff2[50];

        for (double d = -50.0; d < 50.0; d += .00001)
        {
                ::sprintf(buff, "%0.5f", d);
                double val = aton<double>(buff);
                ::sprintf(buff2, "%0.5f", val);
                t.REQUIRE(0 == ::strcmp(buff, buff2));
                if (::strcmp(buff, buff2))
                {
                        ::printf("%s != %s\n", buff, buff2);
                        break;
                }
        }

        t.REQUIRE(aton<float>(subbuffer()) == 0.0);
        subbuffer remainder;
        t.REQUIRE(aton<float>("99A", &remainder) == 99.0);
        t.REQUIRE(remainder.equals('A'));
}

static void test_remainder(wbtester& t)
{
        subbuffer sval1("12345Hello");
        subbuffer rem;
        int ival = aton<int>(sval1, &rem);
        t.REQUIRE(ival == 12345);
        t.REQUIRE(rem.equals("Hello"));

        subbuffer sval2("1234.5Goodbye");
        rem.clear();
        double dval = aton<double>(sval2, &rem);
        t.REQUIRE(dval == double(1234.5));
        t.REQUIRE(rem.equals("Goodbye"));

        subbuffer sval3 = sval1.before('o');
        rem.clear();
        ival = aton<int>(sval3, &rem);
        t.REQUIRE(ival == 12345);
        t.REQUIRE(rem.equals("Hell"));
}

static void test_integer_performance(wbtester&)
{
        std::vector<char*> nums;
        nums.reserve(1000000);

        for (int i = -1147483640; i < 1147483640; i += 3163)
        {
                char* buff = new char[12];
                sprintf(buff, "%d", i);
                nums.push_back(buff);
        }

        uint64_t start, end;
        double total = 0;
        double diff = 0;
        double each = 0;

        start = get_microseconds();
        for (size_t i = 0; i < nums.size(); ++i)
        {
                total += aton<int>(nums[i]);
        }
        end = get_microseconds();
        diff = end - start;
        each = diff / nums.size();

        fprintf(stdout, "%-20s %zu conversions (result %.1f) took %.3fus/check => %14.3f checks/sec\n",
                "aton<int>", nums.size(), total, each, 1000000 / each);

        total = 0;
        start = get_microseconds();
        for (size_t i = 0; i < nums.size(); ++i)
        {
                total += strtol(nums[i], NULL, 10);
        }
        end = get_microseconds();
        diff = end - start;
        each = diff / nums.size();

        fprintf(stdout, "%-20s %zu conversions (result %.1f) took %.3fus/check => %14.3f checks/sec\n",
                "strtol", nums.size(), total, each, 1000000 / each);


        total = 0;
        start = get_microseconds();
        for (size_t i = 0; i < nums.size(); ++i)
        {
                int v;
                sscanf(nums[i], "%d", &v);
                total += v;
        }
        end = get_microseconds();
        diff = end - start;
        each = diff / nums.size();

        fprintf(stdout, "%-20s %zu conversions (result %.1f) took %.3fus/check => %14.3f checks/sec\n",
                "sscanf(%d)", nums.size(), total, each, 1000000 / each);

        for (size_t i = 0; i < nums.size(); ++i)
        {
                delete nums[i];
        }
}

static void test_double_performance(wbtester&)
{
        std::vector<char*> nums;
        for (double d = -25.00001; d < 25.0; d += 0.00007)
        {
                char* buff = new char[11];
                sprintf(buff, "%.5f", d);
                nums.push_back(buff);
        }

        timeval start, end, diff;
        double total = 0;

        gettimeofday(&start, NULL);
        for (size_t i = 0; i < nums.size(); ++i)
        {
                total += aton<double>(nums[i]);
        }
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);

        fprintf(stdout, "%-20s %zu conversions (result %.1f) took %.3fus/check => %13.3f checks/sec\n",
                "aton<double>", nums.size(), total,
                (diff.tv_sec * 1000000.0 + diff.tv_usec) / nums.size(),
                nums.size() / (diff.tv_sec + diff.tv_usec / 1000000.0));

        total = 0;
        gettimeofday(&start, NULL);
        for (size_t i = 0; i < nums.size(); ++i)
        {
                total += strtod(nums[i], NULL);
        }
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);

        fprintf(stdout, "%-20s %zu conversions (result %.1f) took %.3fus/check => %13.3f checks/sec\n",
                "strtod", nums.size(), total,
                (diff.tv_sec * 1000000.0 + diff.tv_usec) / nums.size(),
                nums.size() / (diff.tv_sec + diff.tv_usec / 1000000.0));


        total = 0;
        gettimeofday(&start, NULL);
        for (size_t i = 0; i < nums.size(); ++i)
        {
                double v;
                sscanf(nums[i], "%lf", &v);
                total += v;
        }
        gettimeofday(&end, NULL);
        timersub(&end, &start, &diff);

        fprintf(stdout, "%-20s %zu conversions (result %.1f) took %.3fus/check => %13.3f checks/sec\n",
                "sscanf(%lf)",
                nums.size(), total,
                (diff.tv_sec * 1000000.0 + diff.tv_usec) / nums.size(),
                nums.size() / (diff.tv_sec + diff.tv_usec / 1000000.0));

        for (size_t i = 0; i < nums.size(); ++i)
        {
                delete nums[i];
        }
}

int main(int argc, char** argv)
{
        wbtester t;

        t.ADD_TEST(test_simple);
        t.ADD_TEST(test_overflow);
        t.ADD_TEST(test_extensive);
        t.ADD_TEST(test_floating_point);
        t.ADD_TEST(test_remainder);
        t.ADD_TEST(test_integer_performance);
        t.ADD_TEST(test_double_performance);

        return t.run();
}
