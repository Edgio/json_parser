#include "subbuffer.h"
#include "wbtest.h"

#include <string>

static void test_empty(wbtester& t)
{
        subbuffer sb; // an empty subbuffer
        t.REQUIRE(sb.empty());
        t.REQUIRE(0 == sb.length());
        t.REQUIRE(NULL == sb.begin());

        subbuffer sb2(0, 5);
        t.REQUIRE(sb2.empty());
        t.REQUIRE(0 == sb2.length());
}

static void test_compare(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(0 > sb.compare("hello.worle"));
        t.REQUIRE(0 < sb.compare("Hello.World"));
        t.REQUIRE(0 > sb.compare(CONST_SUBBUF("hello.worle"), CASE_SENSITIVE));
        t.REQUIRE(0 < sb.compare(CONST_SUBBUF("Hello.World"), CASE_SENSITIVE));
        t.REQUIRE(!sb.compare("Hello.World", CASE_INSENSITIVE));
}

static void test_equals(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(!sb.equals(subbuffer("Hello.World")));
        t.REQUIRE(!sb.equals(subbuffer("Hello.World"), CASE_SENSITIVE));
        t.REQUIRE(sb.equals(subbuffer("hello.world"), CASE_SENSITIVE));
        t.REQUIRE(sb.equals(subbuffer("Hello.World"), CASE_INSENSITIVE));

        // now with const char*'s
        t.REQUIRE(!sb.equals("Hello.World", 11));
        t.REQUIRE(sb.equals("Hello.World", 11, CASE_INSENSITIVE));

        subbuffer esb(0, 0);
        t.REQUIRE(!esb.equals(sb));
        t.REQUIRE(!sb.equals(esb));

        subbuffer esb2(0, 5); // length provided but no ptr
        t.REQUIRE(esb.equals(esb2));
        t.REQUIRE(esb2.equals(esb));

        const char* txt = "hello world";
        subbuffer esb3(txt, 0);
        t.REQUIRE(esb.equals(esb3));
        t.REQUIRE(esb3.equals(esb));

        subbuffer e1("world", 0);
        subbuffer e2("hello", 0);
        t.REQUIRE(e1.equals(e2));
        t.REQUIRE(e1 == e2);

        // test equality of char
        subbuffer c1("a");
        t.REQUIRE(c1.equals('a'));
        t.REQUIRE(c1.equals('A', CASE_INSENSITIVE));
        t.REQUIRE(!c1.equals('b'));
        t.REQUIRE(!c1.equals('B', CASE_INSENSITIVE));
}

static void test_requals(wbtester& t)
{
        subbuffer sb("hello.world.hello.world.hello.world.hello.world.hello.world");
        t.REQUIRE(!sb.requals(subbuffer("hello.world.hello.world.hello.world.hello.world.hello.worlD")));
        t.REQUIRE(!sb.requals(subbuffer("hello.world.hello.world.hello.world.hello.world.hello.worl")));
        t.REQUIRE(!sb.requals(subbuffer("1ello.world.hello.world.hello.world.hello.world.hello.world")));
        t.REQUIRE(!sb.requals(subbuffer("ello.world.hello.world.hello.world.hello.world.hello.world")));
        t.REQUIRE(!sb.requals(subbuffer("hello.world.hello.world.hello.world.hello.world.hello.worldd")));
        t.REQUIRE(sb.requals(subbuffer("hello.world.hello.world.hello.world.hello.world.hello.worlD"), CASE_INSENSITIVE));
        t.REQUIRE(!sb.requals(subbuffer("hello.world.hello.world.hello.world.hello.worl4.hello.worlD"), CASE_INSENSITIVE));
}

static void test_starts_with(wbtester& t)
{
        subbuffer sb("hello.world");
        subbuffer unset_sub;
        t.REQUIRE(sb.starts_with(subbuffer("hello")));
        t.REQUIRE(sb.starts_with(subbuffer("HEllo"), CASE_INSENSITIVE));
        t.REQUIRE(sb.starts_with(subbuffer("hello.world")));
        t.REQUIRE(!sb.starts_with(subbuffer("HEllo")));
        t.REQUIRE(!sb.starts_with(subbuffer("hello.world.goodbye")));
        t.REQUIRE(sb.starts_with(subbuffer(""), CASE_SENSITIVE));
        t.REQUIRE(sb.starts_with(subbuffer(""), CASE_INSENSITIVE));
        t.REQUIRE(sb.starts_with(unset_sub, CASE_SENSITIVE));
        t.REQUIRE(sb.starts_with(unset_sub, CASE_INSENSITIVE));

        chargrp cg1("ellh");
        chargrp cg2("ell");
        t.REQUIRE(sb.starts_with(cg1));
        t.REQUIRE(!sb.starts_with(cg2));

        t.REQUIRE(sb.starts_with('h'));
        t.REQUIRE(!sb.starts_with('Q'));

        subbuffer empty("");
        t.REQUIRE(empty.starts_with(subbuffer(""), CASE_SENSITIVE));
        t.REQUIRE(empty.starts_with(subbuffer(""), CASE_INSENSITIVE));
}

static void test_ends_with(wbtester& t)
{
        subbuffer sb("hello.world");
        subbuffer unset_sub;
        t.REQUIRE(sb.ends_with(subbuffer("world")));
        t.REQUIRE(sb.ends_with(subbuffer("WOrld"), CASE_INSENSITIVE));
        t.REQUIRE(sb.ends_with(subbuffer("hello.world")));
        t.REQUIRE(!sb.ends_with(subbuffer("WOrld")));
        t.REQUIRE(!sb.ends_with(subbuffer("hello.world0")));
        t.REQUIRE(sb.ends_with(subbuffer(""), CASE_SENSITIVE));
        t.REQUIRE(sb.ends_with(subbuffer(""), CASE_INSENSITIVE));
        t.REQUIRE(sb.ends_with(unset_sub, CASE_SENSITIVE));
        t.REQUIRE(sb.ends_with(unset_sub, CASE_INSENSITIVE));

        chargrp cg1("abcd");
        chargrp cg2("xnf");
        t.REQUIRE(sb.ends_with(cg1));
        t.REQUIRE(!sb.ends_with(cg2));

        t.REQUIRE(sb.ends_with('d'));
        t.REQUIRE(!sb.ends_with('Q'));

        subbuffer empty("");
        t.REQUIRE(empty.ends_with(subbuffer(""), CASE_SENSITIVE));
        t.REQUIRE(empty.ends_with(subbuffer(""), CASE_INSENSITIVE));
}

static void test_before(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(!sb.before('.').equals(CONST_SUBBUF("Hello")));
        t.REQUIRE(sb.before('.').equals(CONST_SUBBUF("hello")));
        t.REQUIRE(sb.before('.').equals(CONST_SUBBUF("Hello"), CASE_INSENSITIVE));
        t.REQUIRE(sb.before('.').equals(CONST_SUBBUF("Hello"), CASE_INSENSITIVE));

        t.REQUIRE(sb.before(';').equals(CONST_SUBBUF("hello.world"))); // should be whole string

        chargrp cg(".:;");
        t.REQUIRE(sb.before(cg).equals(CONST_SUBBUF("hello")));
        cg.remove('.');
        t.REQUIRE(sb.before(cg).equals(CONST_SUBBUF("hello.world")));

        sb = subbuffer("hello.-.world");
        t.REQUIRE(sb.before(CONST_SUBBUF(".-.")).equals("hello"));
}

static void test_rbefore(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(!sb.rbefore('.').equals(CONST_SUBBUF("Hello")));
        t.REQUIRE(sb.rbefore('.').equals(CONST_SUBBUF("hello")));
        t.REQUIRE(sb.rbefore('.').equals(CONST_SUBBUF("Hello"), CASE_INSENSITIVE));
        t.REQUIRE(sb.rbefore('.').equals(CONST_SUBBUF("Hello"), CASE_INSENSITIVE));

        t.REQUIRE(sb.rbefore(';').equals(CONST_SUBBUF("hello.world"))); // should be whole string

        sb = subbuffer("hello.world.");
        t.REQUIRE(!sb.rbefore('.').equals(CONST_SUBBUF("hello.World")));
        t.REQUIRE(sb.rbefore('.').equals(CONST_SUBBUF("hello.world")));
        t.REQUIRE(sb.rbefore('.').equals(CONST_SUBBUF("Hello.World"), CASE_INSENSITIVE));
        t.REQUIRE(sb.rbefore('.').equals(CONST_SUBBUF("Hello.worLD"), CASE_INSENSITIVE));

        chargrp cg(",a");
        t.REQUIRE(sb.rbefore(cg).equals("hello.world."));
        cg.add('.');
        t.REQUIRE(sb.rbefore(cg).equals("hello.world"));
}

static void test_after(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(!sb.after('.').equals(CONST_SUBBUF("World")));
        t.REQUIRE(sb.after('.').equals(CONST_SUBBUF("world")));
        t.REQUIRE(sb.after('.').equals(CONST_SUBBUF("World"), CASE_INSENSITIVE));
        t.REQUIRE(sb.after('.').equals(CONST_SUBBUF("World"), CASE_INSENSITIVE));

        t.REQUIRE(sb.after(';').empty());  // not found so will be empty

        chargrp cg(",");
        t.REQUIRE(sb.after(cg).empty());
        cg.add('.');
        t.REQUIRE(sb.after(cg).equals(CONST_SUBBUF("world")));

        sb = subbuffer("hello.-.world");
        t.REQUIRE(sb.after(CONST_SUBBUF(".-.")).equals(CONST_SUBBUF("world")));

        subbuffer esb(0, 0);
        t.REQUIRE(esb.after('=').empty());
}

static void test_contains(wbtester& t)
{
        chargrp yes("d");
        chargrp no("Q");
        subbuffer sb("hello.world");
        t.REQUIRE(sb.contains(yes));
        t.REQUIRE(!sb.contains(no));

        t.REQUIRE(sb.contains(subbuffer("o.w")));
        t.REQUIRE(!sb.contains(subbuffer("two")));

        sb = subbuffer(0, 0);
        t.REQUIRE(!sb.contains(yes));
        t.REQUIRE(!sb.contains(no));

        t.REQUIRE(!sb.contains(subbuffer("o.w")));
        t.REQUIRE(!sb.contains(subbuffer("two")));
}

static void test_rafter(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(!sb.rafter('.').equals(CONST_SUBBUF("World")));
        t.REQUIRE(sb.rafter('.').equals(CONST_SUBBUF("world")));
        t.REQUIRE(sb.rafter('.').equals(CONST_SUBBUF("World"), CASE_INSENSITIVE));
        t.REQUIRE(sb.rafter('.').equals(CONST_SUBBUF("World"), CASE_INSENSITIVE));

        t.REQUIRE(sb.rafter(';').empty());  // not found so will be empty

        sb = subbuffer("hello.world.");
        t.REQUIRE(sb.rafter('.').empty());

        chargrp cg("w");
        chargrp cg2("YUM");
        t.REQUIRE(sb.rafter(cg).equals(CONST_SUBBUF("orld.")));
        t.REQUIRE(sb.rafter(cg2).empty());
}

static void test_find_char(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(subbuffer::npos == sb.find('Q'));
        t.REQUIRE(0 == sb.find('h'));
        t.REQUIRE(0 == sb.find('H', 0, CASE_INSENSITIVE));
        t.REQUIRE(6 == sb.find('w'));
        t.REQUIRE(6 == sb.find('w', 2));
        t.REQUIRE(subbuffer::npos == sb.find('w', 7));
        t.REQUIRE(6 == sb.find('W', 0, CASE_INSENSITIVE));
        t.REQUIRE(6 == sb.find('W', 2, CASE_INSENSITIVE));
        t.REQUIRE(subbuffer::npos == sb.find('W', 7, CASE_INSENSITIVE));
        t.REQUIRE(subbuffer::npos == sb.find('d', 12));
}

static void test_rfind_chargrp(wbtester& t)
{
        chargrp cg(";.:");
        subbuffer sb("hello.;world");
        t.REQUIRE(6 == sb.rfind(cg));
        cg.remove(".;");
        t.REQUIRE(subbuffer::npos == sb.rfind(cg));

        sb = subbuffer(0, 0);
        t.REQUIRE(subbuffer::npos == sb.rfind(cg));
}

static void test_rfind_char(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(subbuffer::npos == sb.rfind('Q'));
        t.REQUIRE(0 == sb.rfind('h'));
        t.REQUIRE(0 == sb.rfind('h', sb.length() - 1, CASE_SENSITIVE));
        t.REQUIRE(subbuffer::npos == sb.rfind('q', sb.length() - 1, CASE_SENSITIVE));
        t.REQUIRE(0 == sb.rfind('H', sb.length() - 1, CASE_INSENSITIVE));
        t.REQUIRE(subbuffer::npos == sb.rfind('q', sb.length() - 1, CASE_INSENSITIVE));

        sb = subbuffer(0, 0);
        t.REQUIRE(subbuffer::npos == sb.rfind('h'));
}

static void test_find_string(wbtester& t)
{
        subbuffer sb("hello.world");
        t.REQUIRE(subbuffer::npos == sb.find(subbuffer("hello.world.cruel")));
        t.REQUIRE(subbuffer::npos == sb.find(subbuffer("cruel")));
        t.REQUIRE(0 == sb.find(subbuffer("hello")));
        t.REQUIRE(subbuffer::npos == sb.find(subbuffer("hello"), 2));
        t.REQUIRE(subbuffer::npos == sb.find(subbuffer("Hello")));
        t.REQUIRE(0 == sb.find("Hello", 0, CASE_INSENSITIVE));
        t.REQUIRE(6 == sb.find("World", 0, CASE_INSENSITIVE));
        t.REQUIRE(6 == sb.find(subbuffer("world")));
        t.REQUIRE(subbuffer::npos == sb.find(subbuffer("World")));

        const char* str = "line1\r\nline2\r\nline3\r\nline4\r\n\r\n";
        sb = subbuffer(str, strlen(str));
        t.REQUIRE(subbuffer::npos != sb.find(subbuffer("\r\n\r\n")));
}

static void test_ltrim(wbtester& t)
{
        subbuffer sb("hhhello.world");
        t.REQUIRE(sb.length() == sb.ltrim(' ').length());
        t.REQUIRE(sb.length() - 3 == sb.ltrim('h').length());
        t.REQUIRE(sb.equals("ello.world"));

        sb = subbuffer("hhhello.world");
        t.REQUIRE(sb.length() - 2 == sb.ltrim('h', 2).length());

        sb = subbuffer("hhh");
        t.REQUIRE(sb.ltrim('h', 3).empty());
        sb = subbuffer("hhh");
        t.REQUIRE(sb.ltrim('h').empty());

        sb = subbuffer("1234xyz...");
        chargrp cg(".zyx2314");
        t.REQUIRE(0 == sb.ltrim(cg).length());
        t.REQUIRE(sb.empty());
        t.REQUIRE(0  == sb.ltrim(cg).length());
}

static void test_rtrim(wbtester& t)
{
        subbuffer sb("hello.world12333");
        t.REQUIRE(sb.length() == sb.rtrim(' ').length());
        t.REQUIRE(sb.length() - 3 == sb.rtrim('3').length());
        t.REQUIRE(sb.equals("hello.world12"));
        t.REQUIRE(sb.length() - 1 == sb.rtrim('2').length());
        t.REQUIRE(sb.equals("hello.world1"));
        t.REQUIRE(sb.length() - 1 == sb.rtrim('1').length());
        t.REQUIRE(sb.equals("hello.world"));

        sb = subbuffer(".....");
        t.REQUIRE(sb.rtrim('.').empty());

        sb = subbuffer("   ");
        t.REQUIRE(sb.rtrim(isspace).empty());
        t.REQUIRE(sb.rtrim(isspace).empty());

        chargrp cg(" abc");
        sb = subbuffer("aaa   bbb    ccc   ");
        t.REQUIRE(sb.rtrim(cg).empty());
        t.REQUIRE(sb.rtrim(cg).empty());
}

static void test_trim(wbtester& t)
{
        subbuffer sb("hhhello.worldhhh");
        t.REQUIRE(sb.trim(' ').equals("hhhello.worldhhh"));
        t.REQUIRE(sb.trim('h').equals("ello.world"));

        sb = subbuffer("hhhhhhhhhh");
        t.REQUIRE(sb.trim('h').empty());
        t.REQUIRE(sb.trim('h').empty());

        sb = subbuffer("   hello   ");
        t.REQUIRE(sb.trim(isspace).equals("hello"));

        sb = subbuffer("   ");
        t.REQUIRE(sb.trim(isspace).empty());
        t.REQUIRE(sb.trim(isspace).empty());

        chargrp cg("/ ");
        sb = subbuffer("/dir1/dir2/ ");
        t.REQUIRE(sb.trim(cg).equals("dir1/dir2"));
}

static void test_sub(wbtester& t)
{
        subbuffer sb("123456789");
        t.REQUIRE(sb.sub(0).equals("123456789"));
        t.REQUIRE(sb.sub(4).equals("56789"));
        t.REQUIRE(sb.sub(9).empty());
}

static void test_lt(wbtester& t)
{
        subbuffer sb1("hell");
        subbuffer sb2("hello");
        subbuffer sb3("abc");
        subbuffer sb4("xyz");
        subbuffer e1("");
        subbuffer e2(0, 0);

        t.REQUIRE(sb1 < sb2);
        t.REQUIRE(sb1 < sb4);
        t.REQUIRE(sb2 < sb4);
        t.REQUIRE(sb3 < sb4);
        t.REQUIRE(!(sb4 < sb3));
        t.REQUIRE(!(e1 < e2));
}

static void test_is_sub_of(wbtester& t)
{
        subbuffer parent1("Marry ate some well done lamb");
        subbuffer parent2("The best bacon comes from talking pigs");

        subbuffer sb = parent1.sub(6, 8);
        t.REQUIRE(sb.is_sub_of(parent1));
        t.REQUIRE(!sb.is_sub_of(parent2));

        sb.advance(4);
        t.REQUIRE(sb.is_sub_of(parent1));
        t.REQUIRE(!sb.is_sub_of(parent2));

        sb.advance(400);  // will clear the subbuffer
        t.REQUIRE(!sb.is_sub_of(parent1));
        t.REQUIRE(!sb.is_sub_of(parent2));

        sb = parent2.sub(0, 14);
        t.REQUIRE(!sb.is_sub_of(parent1));
        t.REQUIRE(sb.is_sub_of(parent2));

        sb = parent2.sub(0, 1);
        t.REQUIRE(!sb.is_sub_of(parent1));
        t.REQUIRE(sb.is_sub_of(parent2));

        sb = parent2.sub(0, 0);   // the internal ptr will have a value, length will be 0
        t.REQUIRE(!sb.is_sub_of(parent1));
        t.REQUIRE(sb.is_sub_of(parent2));
}

static bool is_a_char(char c) { return c == 'a'; }
static bool is_a_int(int c) { return c == 'a'; }
static bool is_numb(int c) { return c >= '0' && c <= '9'; }
class is_upper_case
{
public:
        bool operator()(char i)
        {
                return i > 64 && i < 91;
        }
};

static void test_is_all(wbtester& t)
{
        subbuffer aaaa("aaaa");
        subbuffer bbbb("bbbb");
        subbuffer BBBB("BBBB");
        subbuffer BBBb("BBBb");

        t.REQUIRE(aaaa.is_all(is_a_char));
        t.REQUIRE(aaaa.is_all(is_a_char));

        t.REQUIRE(!bbbb.is_all(is_a_int));
        t.REQUIRE(!bbbb.is_all(is_a_int));

        t.REQUIRE(BBBB.is_all(is_upper_case()));
        t.REQUIRE(!BBBb.is_all(is_upper_case()));

        subbuffer sb("aaaabbbbccccaaaa");
        t.REQUIRE(sb.sub(12).is_all(is_a_char));
        t.REQUIRE(!sb.sub(8).is_all(is_a_char));

        sb = subbuffer("abcd1234");
        t.REQUIRE(!sb.is_all(is_numb));
        t.REQUIRE(sb.is_all(is_numb, 4));
        t.REQUIRE(sb.sub(4).is_all(is_numb));
}

static void test_remove_from(wbtester& t)
{
        subbuffer sb("aaaabbbb1234");
        t.REQUIRE(sb.remove_from(8).equals("aaaabbbb"));
        t.REQUIRE(sb.remove_from(4).equals("aaaa"));
        t.REQUIRE(sb.remove_from(40).equals("aaaa"));
}

static void test_is_set(wbtester& t)
{
        // a subbuffer is only considered set if the const char* is non NULL.

        // the use of std::string in this test points out a diff in logic
        // subbuffer is calling std::string::c_str to get the ptr to the const char*
        // std::string::c_str always returns a valid ptr so it is always considered set.

        subbuffer sb1;
        t.REQUIRE(!sb1.is_set());

        std::string buff;
        subbuffer sb2(buff);
        t.REQUIRE(sb2.is_set());

        buff = "Hello";
        subbuffer sb3(buff);
        t.REQUIRE(sb3.is_set());

        sb3.clear();
        t.REQUIRE(!sb3.is_set());

        sb3.set("Hello World");
        t.REQUIRE(sb3.is_set());

        sb3.clear();
        t.REQUIRE(!sb3.is_set());

        sb3.set("Hello World", 5);  // set it to the first 5 chars
        t.REQUIRE(sb3.is_set());

        buff.clear();
        subbuffer sb4(buff);
        t.REQUIRE(sb4.is_set());
}

static void test_copy_to(wbtester& t)
{
        subbuffer sb1("Hello World, where are you");
        char buff[100];
        sb1.after(' ').before(',').copy_to(buff);
        t.REQUIRE(!strcmp(buff, "World"));

        sb1.after(',').ltrim(' ').copy_to(buff);
        t.REQUIRE(!strcmp(buff, "where are you"));

        sb1.after(',').ltrim(' ').copy_to(buff, 5);
        t.REQUIRE(!strcmp(buff, "where"));
}

static void test_aton(wbtester& t)
{
        subbuffer sb1("1048575Mary");
        subbuffer sb2("fffff");
        subbuffer sb3("1234.567");
        subbuffer sb4("-1234");
        subbuffer sb5("-1234.567");
        subbuffer sb_empty;
        subbuffer sb_u64_max("18446744073709551615");
        subbuffer sb_u64_over("18446744073709551617");


        int ival(0);
        unsigned int uival(0);
        int64_t i64val(0);
        uint64_t ui64val(0);
        short sval(0);
        double dval(0.0);
        subbuffer rem;

        t.REQUIRE(sb1.aton(ival, &rem));
        t.REQUIRE(1048575 == ival);
        t.REQUIRE(rem.equals("Mary"));
        t.REQUIRE(sb1.before('y').aton(ival, &rem));
        t.REQUIRE(1048575 == ival);
        t.REQUIRE(rem.equals("Mar"));
        t.REQUIRE(sb1.aton(uival));
        t.REQUIRE(1048575 == uival);
        t.REQUIRE(sb1.aton(ui64val));
        t.REQUIRE(1048575 == ui64val);
        t.REQUIRE(!sb1.aton(sval));                      // overflow
        t.REQUIRE(sb2.aton(ival, NULL, 16));
        t.REQUIRE(1048575 == ival);
        t.REQUIRE(!sb2.aton(sval, NULL, 16));            // overflow
        t.REQUIRE(sb3.aton(sval));
        t.REQUIRE(1234 == sval);
        t.REQUIRE(sb3.aton(dval));
        t.REQUIRE(1234.567 == dval);
        t.REQUIRE(!sb_empty.aton(ival));                // empty string
        t.REQUIRE(!sb4.aton(uival));                    // underflow
        t.REQUIRE(sb5.aton(dval));
        t.REQUIRE(-1234.567 == dval);

        t.REQUIRE(sb_u64_max.aton(ui64val));
        t.REQUIRE(18446744073709551615lu == ui64val);
        t.REQUIRE(!sb_u64_max.aton(i64val));

        t.REQUIRE(!sb_u64_over.aton(ui64val));
}

static void test_count(wbtester& t)
{
        subbuffer sb("Hello World");
        t.REQUIRE(sb.count('l') == 3);
        t.REQUIRE(sb.count(':') == 0);

        subbuffer sb2;
        t.REQUIRE(sb2.count('l') == 0);  // test on empty subbuffer
}

static void test_at(wbtester& t)
{
        subbuffer sb("Hello World again", 11);
        t.REQUIRE(sb.at(5) == ' ');
        t.REQUIRE(sb.at(12) == char(0));
}

static void test_index(wbtester& t)
{
        subbuffer sb("Hello World again", 11);
        t.REQUIRE(sb[5] == ' ');
        t.REQUIRE(sb[12] == 'a'); // is not bounds checked so will allow you to overstep the bounds
}

static void test_hash(wbtester& t)
{
        const char* hw1 = "Hello_World.mp4...";
        const char* hw2 = "Hello_World.mp4......"; // a little longer to get unique ptr
        const char* hw3 = "hello_world.mp4...";
        const char* hw4 = "hello_world.mp4......";
        const char* first = "hello_";
        const char* last = "world.mp4";
        subbuffer sb1(hw1, 15);
        subbuffer sb2(hw2, 15);
        subbuffer sb3(hw3, 15);
        subbuffer sb4(hw4, 15);
        subbuffer sb_first(first);
        subbuffer sb_last(last);
        t.REQUIRE(sb2.hash() == sb2.hash());
        t.REQUIRE(sb2.hash() != sb3.hash()); // diff case
        t.REQUIRE(sb2.hash(0, CASE_INSENSITIVE) == sb3.hash()); // diff case
        t.REQUIRE(sb3.hash() == sb_last.hash(sb_first.hash()));
}

int main(int argc, char** argv)
{
        wbtester t;

        t.ADD_TEST(test_empty);
        t.ADD_TEST(test_compare);
        t.ADD_TEST(test_equals);
        t.ADD_TEST(test_requals);
        t.ADD_TEST(test_starts_with);
        t.ADD_TEST(test_ends_with);
        t.ADD_TEST(test_before);
        t.ADD_TEST(test_rbefore);
        t.ADD_TEST(test_after);
        t.ADD_TEST(test_rafter);
        t.ADD_TEST(test_find_char);
        t.ADD_TEST(test_contains);
        t.ADD_TEST(test_rfind_char);
        t.ADD_TEST(test_find_string);
        t.ADD_TEST(test_ltrim);
        t.ADD_TEST(test_rtrim);
        t.ADD_TEST(test_trim);
        t.ADD_TEST(test_rfind_chargrp);
        t.ADD_TEST(test_sub);
        t.ADD_TEST(test_lt);
        t.ADD_TEST(test_is_sub_of);
        t.ADD_TEST(test_is_all);
        t.ADD_TEST(test_remove_from);
        t.ADD_TEST(test_is_set);
        t.ADD_TEST(test_copy_to);
        t.ADD_TEST(test_aton);
        t.ADD_TEST(test_count);
        t.ADD_TEST(test_at);
        t.ADD_TEST(test_index);
        t.ADD_TEST(test_hash);

        return t.run();
}
