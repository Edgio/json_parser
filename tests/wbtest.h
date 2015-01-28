#ifndef WB_TESTER_H
#define WB_TESTER_H

#include <utility>
#include <vector>

class wbtester
{
public:
        wbtester() : m_name(NULL), m_tests(0), m_failed(0), m_funcs() {}

        void add_test(const char* test_name, void (*func)(wbtester&))
        {
                m_funcs.push_back(std::make_pair(test_name, func));
        }

        int run()
        {
                std::vector<const char*> failed_tests;
                for (std::vector<std::pair<const char*, void (*)(wbtester&) > >::iterator iter = m_funcs.begin();
                     iter != m_funcs.end();
                     iter++)
                {
                        uint32_t start_failed = m_failed;
                        m_name = iter->first;
                        iter->second(*this);
                        if (start_failed != m_failed)
                                failed_tests.push_back(iter->first); 
                }

                if (!m_failed)
                {
                        fprintf(stderr, "%u/%u assertions passed in %zu test functions\n",
                                m_tests - m_failed,
                                m_tests,
                                m_funcs.size());
                        return 0;
                }

                fprintf(stderr, "%u/%u assertions passed in %zu test functions -- %u tests FAILED in %zu test functions\n",
                        m_tests - m_failed,
                        m_tests,
                        m_funcs.size(),
                        m_failed,
                        failed_tests.size());

                for (std::vector<const char*>::iterator iter = failed_tests.begin();
                     iter != failed_tests.end();
                     iter++)
                {
                        fprintf(stderr, "\t%s\n", *iter);
                }

                // test_find_string: whitebox-subbuffer.cc:283: expected 'subbuffer::npos == sb.find(subbuffer("\r\n\r\n"))'
                // 226/227 assertions passed in 27 test functions -- 1 tests FAILED in 1 test functions
                //      test_find_string

                return 1;
        }

        bool require(const char* file, const int line, const char* expr, const bool exprval)
        {
                ++m_tests;
                if (exprval) return exprval;
                fprintf(stderr, "%s: %s:%d: expected '%s'\n", m_name, file, line, expr);
                ++m_failed;
        }
private:
        const char* m_name;
        uint32_t m_tests;
        uint32_t m_failed;
        std::vector<std::pair<const char*, void (*)(wbtester&) > > m_funcs;
};

#define ADD_TEST(x) add_test(#x, x)
#define REQUIRE(x) require(__FILE__, __LINE__, #x, (x))

#endif
