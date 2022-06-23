/*********** -*- mode: c++; c-file-style: "sailfish"; -*- **********
 *
 * Copyright (c) 2011-2012 Edgio Networks, Inc.
 * All Rights Reserved
 *
 ****************************************************************/

#include "aton.h"
#include "chargrp.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <map>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> s_equal_strings;
vector<string> s_equal_lower_strings;
vector<string> s_unequal_strings;
vector<subbuffer> s_equal_subbuffers;
vector<subbuffer> s_equal_lower_subbuffers;
vector<subbuffer> s_unequal_subbuffers;

namespace preds
{
        struct is_space
        {
        public:
                inline bool operator()(char c) const
                {
                        return ' ' == c || '\n' == c || '\t' == c;
                }
        };
};

uint64_t get_ns()
{
        timeval tv;
        ::gettimeofday(&tv, 0);
        return static_cast <uint64_t>((static_cast <uint64_t>(tv.tv_sec) * 1000000)  + tv.tv_usec) * 1000;
}

const char* buff = "Mary ate her lamb as stew with a biscuit";

void test1(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                int res = ::memcmp(src1.begin(), src2.begin(), 8);
                if (res) printf("failed\n");
        }
}

void test2(uint32_t cnt, subbuffer src1)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                const void* pos = memchr(src1.begin(), ':', src1.length());
                if (pos) printf("wow, found something\n");
        }
}

void test3(uint32_t cnt, subbuffer src1)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                const char* pos = strchr(src1.begin(), ':');
                if (pos) printf("wow, found something\n");
        }
}

void test4(uint32_t cnt, subbuffer src1)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                const char* pos = NULL;
                const char* beg = src1.begin();
                const char* end = beg + (src1.length() - 1);
                while (beg <= end)
                {
                        if (*beg == ':')
                        {
                                pos = beg;
                                break;
                        }
                        ++beg;
                }
                if (pos) printf("wow, found something\n");
        }
}

void test100(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                if (!src1.starts_with(src2))
                        printf("Does not start with\n");
        }
}

void test101(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        subbuffer src3(src1.begin(), src1.length());
        for (uint32_t i = 0; i < cnt; i++)
        {
                if (!src3.starts_with(src2))
                        printf("Does not start with\n");
        }
}

void test102(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.starts_with(src2, CASE_INSENSITIVE))
                        printf("Does not start with\n");
}

void test103(uint32_t cnt, subbuffer src1)
{
        preds::is_space isspace;
        for (uint32_t i = 0; i < cnt; i++)
                if (src1.starts_with(isspace))
                        printf("starts_with white_space, hmm\n");
}

void test104(uint32_t cnt, subbuffer src1)
{
        static chargrp cg(" \t\n/");
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.starts_with(cg))
                {
                        printf("does not starts_with cg, hmm\n");
                        printf("src1: %.*s, chargrp: ' \t\n/'\n", SUBBUF_FORMAT(src1));
                        exit(0);
                }
}

void test105(uint32_t cnt, subbuffer src1)
{
        static chargrp cg(" \t\n");
        for (uint32_t i = 0; i < cnt; i++)
                if (src1.starts_with(cg))
                        printf("starts_with cg, hmm\n");
}

void test200(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.equals(src2))
                        printf("Does not equal\n");
}

void test201(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        subbuffer src3(src1.begin(), src1.length());
        for (uint32_t i = 0; i < cnt; i++)
                if (!src3.equals(src2))
                        printf("Does not equal\n");
}

void test202(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                if (!src1.equals(src2, CASE_INSENSITIVE))
                        printf("Does not equal\n");
        }
}

void test203(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (src1.requals(src2))
                        printf("equals\n");
}

void test204(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.equals(src2))
                        printf("Does not equal\n");
}

void test205(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (src1.equals(src2))
                        printf("equals\n");
}

void test206(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.requals(src2))
                        printf("Does not equal\n");
}

void test300(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (0 == src1.compare(src2))
                        printf("compare returned zero\n");
}

void test301(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (0 != src1.compare(src2))
                        printf("compare returned non zero\n");
}

void test302(uint32_t cnt)
{
        // compare equal strings in caseless manner
        for (uint32_t i = 0; i < cnt; i++)
                if (0 != s_equal_lower_subbuffers[i].compare(s_equal_subbuffers[(i+1)], CASE_INSENSITIVE))
                        printf("compare returned non zero\n");
}

void test303(uint32_t cnt)
{
        subbuffer sb("Accept-Encoding");
        // compare equal strings in caseless manner
        for (uint32_t i = 0; i < cnt; i++)
                if (0 == s_equal_subbuffers[i].compare(sb, CASE_INSENSITIVE))
                        printf("compare returned zero, shouldn't have matched\n");
}

void test400(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (subbuffer::npos == src1.find(src2))
                        printf("Did not find %.*s in %.*s\n", SUBBUF_FORMAT(src2), SUBBUF_FORMAT(src1));
}

void test401(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        subbuffer src3(src1.begin(), src1.length());
        for (uint32_t i = 0; i < cnt; i++)
                if (subbuffer::npos == src3.find(src2, 0, CASE_INSENSITIVE))
                        printf("Did not find\n");
}

void test402(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (subbuffer::npos == src1.find(src2, 0, CASE_INSENSITIVE))
                        printf("Did not find\n");
}

void test403(uint32_t cnt, subbuffer src1, char c)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                size_t pos = src1.rfind(c);
                if (subbuffer::npos == pos) printf("failed\n");
        }
}

void test404(uint32_t cnt, subbuffer src1, char c)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                size_t pos = src1.rfind(c, subbuffer::npos, CASE_INSENSITIVE);
                if (subbuffer::npos == pos) printf("failed\n");
        }
}

void test405(uint32_t cnt, subbuffer src1)
{
        chargrp cg(":\t");
        for (uint32_t i = 0; i < cnt; i++)
                if (subbuffer::npos != src1.find(cg))
                        printf("found cg, hmm\n");
}

void test500(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (src1 < src2)
                        printf("src1 is less than src2\n");
}

void test501(uint32_t cnt, subbuffer src1, subbuffer src2)
{
        for (uint32_t i = 0; i < cnt; i++)
                if (src1 < src2)
                        printf("src1 is less than src2\n");
}

void test502(uint32_t cnt, subbuffer src, char needle, bool should_be_empty)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                subbuffer sb = src.after(needle);
                if (should_be_empty != sb.empty())
                        printf("subbuffer::after returned unexpected results\n");
        }
}

void test504(uint32_t cnt, subbuffer src, char totrim, size_t delta)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                subbuffer sb(src);
                sb.ltrim(totrim);
                if (src.length() - sb.length() != delta)
                        printf("subbuffer::ltrim returned unexpected results\n");
        }
}

#if __cplusplus > 199711L

void test600(uint32_t cnt, subbuffer src1)
{
        std::regex re("dca|ord|oxr");
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.match(re))
                        printf("regex did not match, hmm\n");
}

void test601(uint32_t cnt, subbuffer src1)
{
        std::regex re("dca|ord|oxr");
        for (uint32_t i = 0; i < cnt; i++)
                if (!src1.search(re))
                        printf("regex did not match, hmm\n");
}

#endif


static std::string buff2("Mary ate some lamb with rice and beans");

struct results
{
        results(uint32_t t, subbuffer st, uint32_t c, uint64_t tot, double e)
                :type(t), stype(st), cnt(c), total(tot), each(e)
        {}
        uint32_t type;
        subbuffer stype;
        uint32_t cnt;
        uint64_t total;
        double   each;
};

std::vector<results> s_results;

bool run_test (uint32_t type, uint32_t cnt)
{
        std::string header1a("Connection");
        std::string header1b("Connection");
        std::string header2a("Host");
        std::string header2b("Host");
        std::string header3a("Cache-Control");
        std::string header3b("Cache-Control");
        std::string buff1a("Mary ate some lamb with rice and beans");
        std::string buff1b("Mary ate some lamb with rice and beans");
        std::string buff1c("Mary ate some Lamb with rice and beans");
        std::string buff3a("Mary ate");
        std::string buff3b("Mary ate");
        std::string buff4a("LAMB");
        std::string buff4b("lamb");
        std::string buff5a("X-My-Custom-Header: Hello World");
        std::string buff6a("/cust/dir/somelongdirdir/somelongdirdir/somelongdirdir/somelongdirdir/dirwithnumbers123/file1.home.mpg");
        std::string buff6ab("/cust/dir/somelongdirdir/somelongdirdir/somelongdirdir/somelongdirdir/dirwithnumbers123/file1.home.mpg");
        std::string buff6b("/cust/dir/somelongdirdir/somelongdirdir/somelongdirdir/somelongdirdir/dirwithnumbers123/file2.home.mpg");
        std::string buff7a("123_key");
        std::string buff7b("124_key");
        std::string buff7c("223_key");
        std::string buff12a("Mary ate some lamb with rice and beans");
        std::string buff12b("Mary ate some lamb with rice and beans");
        std::string pop1("oxr");
        std::string pop2("dfw");
        std::string pop3("lhr");


        subbuffer stype;

        uint64_t start = get_ns();
        switch (type)
        {
//
// stdlib    1-99
//
        case 1:
                test1(cnt, buff1a, buff1b); stype = subbuffer("memcmp"); break;
        case 2:
                test2(cnt, buff1a); stype = subbuffer("memchr"); break;
        case 3:
                test3(cnt, buff1a); stype = subbuffer("strchr"); break;
        case 4:
                test4(cnt, buff1a); stype = subbuffer("basic loop"); break;

//
// starts_with   100-199
//
        case 100:
                test100(cnt, buff1a, buff3a); stype = subbuffer("starts_with(subbuffer)"); break;
        case 101:
                test101(cnt, buff1a, buff3a); stype = subbuffer("starts_with(subbuffer)"); break;
        case 102:
                test102(cnt, buff1a, buff3a); stype = subbuffer("starts_with(subbuffer) caseless"); break;
        case 103:
                test103(cnt, buff6a); stype = subbuffer("starts_with BinaryPredicate"); break;
        case 104:
                test104(cnt, buff6a); stype = subbuffer("starts_with(chargrp), positive"); break;
        case 105:
                test105(cnt, buff6a); stype = subbuffer("starts_with(chargrp)"); break;

//
// equals   200-299
//
        case 200:
                test200(cnt, buff3a, buff3b); stype = subbuffer("equals"); break;
        case 201:
                test201(cnt, buff3a, buff3b); stype = subbuffer("equals"); break;
        case 202:
                test202(cnt, buff3a, buff3b); stype = subbuffer("equals caseless"); break;
        case 203:
                test203(cnt, buff6a, buff6b); stype = subbuffer("!requals (buff6)"); break;
        case 204:
                test204(cnt, buff6a, buff6ab); stype = subbuffer("equals (buff6)"); break;
        case 205:
                test205(cnt, buff6a, buff6b); stype = subbuffer("!equals (buff6)"); break;
        case 206:
                test206(cnt, buff6a, buff6a); stype = subbuffer("requals (buff6)"); break;

//
// compare  300-399
//
        case 300:
                test300(cnt, header1a, header2a); stype = subbuffer("compare not equal"); break;
        case 301:
                test301(cnt, header1a, header1b); stype = subbuffer("compare is equal"); break;
        case 302:
                test302(cnt); stype = subbuffer("compare caseless is equal"); break;
        case 303:
                test303(cnt); stype = subbuffer("compare caseless early not equal"); break;

//
// find 400-499
//
        case 400:
                test400(cnt, CONST_SUBBUF("Mary had a lamb"), CONST_SUBBUF("had ")); stype = subbuffer("find"); break;
        case 401:
                test401(cnt, buff1a, buff4a); stype = subbuffer("find(subbuffer) caseless"); break;
        case 402:
                test402(cnt, buff1a, buff4a); stype = subbuffer("find(subbuffer) caseless"); break;
        case 403:
                test403(cnt, buff5a, ':' ); stype = subbuffer("rfind"); break;
        case 404:
                test404(cnt, buff5a, 'e'); stype = subbuffer("rfind caseless"); break;
        case 405:
                test405(cnt, buff1a); stype = subbuffer("find(chargrp)"); break;

//
// Misc 500-599
//
        case 500:
                test500(cnt, buff7b, buff7a); stype = subbuffer("operator< subbuffer"); break;
        case 501:
                test501(cnt, buff7c, buff7a); stype = subbuffer("operator< subbuffer"); break;
        case 502:
                test502(cnt, CONST_SUBBUF("key=val"), '=', false); stype = subbuffer("after, positive"); break;
        case 503:
                test502(cnt, CONST_SUBBUF("keyval"), '=', true); stype = subbuffer("after, negative"); break;
        case 504:
                test504(cnt, CONST_SUBBUF("keyval"), ' ', 0); stype = subbuffer("ltrim char, 0"); break;
        case 505:
                test504(cnt, CONST_SUBBUF("  keyval"), ' ', 2); stype = subbuffer("ltrim char, 2"); break;

#if __cplusplus > 199711L
//
// std::regex      600-699
//
        case 600:
                test600(cnt, pop1), stype = subbuffer("match(regex)"); break;
        case 601:
                test601(cnt, pop1), stype = subbuffer("search(regex)"); break;
#endif

        default:
                return false;
        }
        uint64_t end = get_ns();
        uint64_t total = (end - start);
        double each = double(total) / cnt;
        s_results.push_back(results(type, stype, cnt, total, each));
        return true;
}

void print_time(uint64_t ns1, uint64_t ns2, uint32_t cnt, const char* desc)
{
        uint64_t total = ns2 - ns1;;
        double each = double(total) / cnt;
        ::printf("%s. cnt: %u, total ns: %lu, each ns: %0.4f\n", desc, cnt, total, each);
}

void test_memchr(uint32_t cnt, const char* ptr, char c)
{
        uint32_t len = strlen(ptr);
        for (uint32_t i = 0; i < cnt; i++)
        {
                const void* p = memchr(ptr, c, len);
                if (!p)
                        printf("Failed to find %c in %s\n", c, ptr);
        }
}

void test_find_char(uint32_t cnt, subbuffer sb, char c)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                size_t pos = sb.find(c);
                if (subbuffer::npos == pos)
                        printf("Failed to find %c in %.*s\n", c, SUBBUF_FORMAT(sb));
        }
}

void test_std_find(uint32_t cnt, const char* ptr, char c)
{
        uint32_t len = strlen(ptr);
        for (uint32_t i = 0; i < cnt; i++)
        {
                const char* p = std::find(ptr, ptr + len, c);
                if (!p)
                        printf("Failed to find %c in %s\n", c, ptr);
        }
}

void test_memmem(uint32_t cnt, const char* ptr, const char* needle)
{
        uint32_t len = strlen(ptr);
        uint32_t nlen = strlen(needle);
        for (uint32_t i = 0; i < cnt; i++)
        {
                const void* p = memmem(ptr, len, needle, nlen);
                if (!p)
                        printf("Failed to find %s in %s\n", needle, ptr);
        }
}

void test_sb_find_str(uint32_t cnt, subbuffer sb, subbuffer needle)
{
        for (uint32_t i = 0; i < cnt; i++)
        {
                size_t pos = sb.find(needle);
                if (subbuffer::npos == pos)
                        printf("Failed to find %.*s in %.*s\n", SUBBUF_FORMAT(needle), SUBBUF_FORMAT(sb));
        }
}

void test_std_search(uint32_t cnt, const char* ptr, const char* needle)
{
        uint32_t len = strlen(ptr);
        uint32_t nlen = strlen(needle);
        for (uint32_t i = 0; i < cnt; i++)
        {
                const char* p = std::search(ptr, ptr + len, needle, needle + nlen);
                if (!p)
                        printf("Failed to find %s in %s\n", needle, ptr);
        }
}

bool run_comparisons(subbuffer compare, uint32_t cnt)
{
        uint64_t ns = 0;
        if (compare.equals("find_char"))
        {
                ns = get_ns();
                test_memchr(cnt, "Some key: Some value", ':');
                print_time(ns, get_ns(), cnt, "memchr, 9 chars");

                ns = get_ns();
                test_find_char(cnt, CONST_SUBBUF("Some key: Some value"), ':');
                print_time(ns, get_ns(), cnt, "subbuffer::find(char), 9 chars");

                ns = get_ns();
                test_std_find(cnt, "Some key: Some value", ':');
                print_time(ns, get_ns(), cnt, "std::find(char), 9 chars");
        }
        else if (compare.equals("find_str"))
        {
                const char* str = "Some key: Some value";
                ns = get_ns();
                test_memmem(cnt, str, "value");
                print_time(ns, get_ns(), cnt, "memmem, 16 chars");

                ns = get_ns();
                test_sb_find_str(cnt, str, "value");
                print_time(ns, get_ns(), cnt, "subbuffer::find(subbuffer), 16 chars");

                ns = get_ns();
                test_std_search(cnt, str, "value");
                print_time(ns, get_ns(), cnt, "std::search(str), 16 chars");
        }
        return true;
}

void show_results()
{
        size_t max_len = 0;
        for (std::vector<results>::iterator iter = s_results.begin();
             iter != s_results.end();
             iter++)
        {
                max_len = std::max(max_len, (*iter).stype.length());
        }
        char fmt[200];
        sprintf(fmt,"%%-3u. %%-%zu.*s, cnt: %%u, total ns: %%8lu, each ns: %%8.4f\n", max_len); 
        for (std::vector<results>::iterator iter = s_results.begin();
             iter != s_results.end();
             iter++)
        {
                results r = *iter;
                ::printf(fmt, r.type, SUBBUF_FORMAT(r.stype), r.cnt, r.total, r.each);
        }
}

int main (int argc, char** argv)
{
        subbuffer compare;
        uint32_t type = 0;
        uint32_t cnt = 100000;
        for (int i = 1; i < argc; i++)
        {
                subbuffer arg(argv[i]);
                subbuffer key = arg.before('=');
                subbuffer val = arg.after('=');
                if (key.equals("--type"))
                        type = aton<uint32_t>(val);
                else if (key.equals("--cnt"))
                        cnt = aton<uint32_t>(val);
                else if (key.equals("--compare"))
                        compare = val;
        }

        char tmp[50];
        for (int i = 0; i < 1000; i++)
        {
                if (i % 2 == 0)
                        s_equal_strings.push_back(string("Cache-Control"));
                else
                        s_equal_strings.push_back(string("cachE-controL"));
                s_equal_lower_strings.push_back(string("cache-control"));
                sprintf(tmp, "Cache-%d-Control", i);
                s_unequal_strings.push_back(string(tmp));
                s_unequal_subbuffers.push_back(s_unequal_strings.back());
        }
        for (uint64_t i = 0; i < cnt + 1; i++)
        {
                s_equal_subbuffers.push_back(s_equal_strings[rand()%s_equal_strings.size()]);
                s_equal_lower_subbuffers.push_back(s_equal_lower_strings[rand()%s_equal_lower_strings.size()]);
        }

        if (type)
        {
                run_test(type, cnt);
        }
        else if (!compare.empty())
        {
                run_comparisons(compare, cnt);
        }
        else
        {
                for (int i = 1; i < 1000; i++)
                {
                        run_test(i, cnt);
                }
                show_results();
        }
}
