//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    subbuffer.h
//: \details: Object oriented handling of const char* + length (range).
//: \author:  Donnevan "Scott" Yeager
//: \date:    06/21/2013
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

#ifndef _SUBBUFFER_H_
#define _SUBBUFFER_H_

#include "aton_internal.h"
#include "chargrp.h"

#include <algorithm>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/types.h>

#define SUBBUF_LEN(x) ((x).begin()), ((x).length())

#define CONST_SUBBUF(x) subbuffer(x, sizeof(x) - 1)

/// use as in printf("%.*s", SUBBUF_FORMAT(mysubbuf))
#define SUBBUF_FORMAT(x) int((x).length()), ((x).begin())

/**
  @brief An enum for specifying case in comparisons
 */
enum case_sensitivity_t { CASE_INSENSITIVE, CASE_SENSITIVE };

/**
  @brief Non-owned reference to a (possibly not null-terminated) substring

  Use to avoid unnecessary copies of substrings.

  Use to simplify complicated code.

  @note Remember that the source bytes are not copied into the subbuffer object and so
	the subbuffer is only valid for the lifetime of the original string.
 */
class subbuffer
{
public:
        static const size_t npos = ULONG_MAX;

public:
        /**
          @brief Constructor, the subbuffer is empty after this constructor.
         */
        subbuffer() :
                m_p(NULL), m_len(0)
        {}

        /**
          @brief Constructor
          @param [in] src A std::string that may or may not have content."
         */
        subbuffer(const std::string& src) :
                m_p(src.c_str()), m_len(src.length())
        {}

        /**
          @brief Constructor
          @param [in] p Pointer to zero or more chars."
          @param [in] len The number of chars that p points to. Defaults to subbuffer::npos."
         */
        subbuffer(const char* p, size_t len = npos)
                :
                m_p(p),
                m_len(m_p ? len : 0)
        {
                if (m_p && m_len == npos) m_len = strlen(m_p);
                if (m_len == npos) m_len = 0;
        }

        /**
          @brief Used to assign a new substring.
          @param [in] p Pointer to zero or more chars."
          @param [in] len The number of chars that p points to."
         */
        inline void set(const char* p, size_t len)
        {
                m_p = p;
                m_len = len;
                if (npos == m_len) m_len = 0;
        }

        /**
          @brief Used to assign a new substring.
          @param [in] sz Pointer to NULL terminated char array."
         */
        inline void set(const char* sz)
        {
                m_p = sz;
                m_len = (sz ? strlen(sz) : 0);
        }

        /**
          @brief Accessor for the underlying const char*

          @note Do not assume that the buffer is NULL terminated.

          This method was purposely named "begin" rather than "c_str" to
          remind the caller that the returned char* is not guaranteed to
          be NULL terminated.
         */
        inline const char* begin() const { return m_p; }

        /**
          @brief Access the underlying chars as an array.
          @param [in] i The index into the array.

          @note There is no bounds checking done in this method. If you need bounds checking use subbuffer::at(int i).
         */
        inline char operator[](int i) const { return m_p[i]; }

        /**
          @brief Accessor for the length of the buffer
         */
        inline size_t length() const { return m_len; }

        /**
          @brief Does the subbuffer have any characters in it?
          @returns false if the const ptr is NULL or has zero chars in it.
         */
        inline bool empty() const { return !m_len; }

        /**
          @brief Allows the distinction between having a pointer with zero length and having no pointer.
         */
        inline bool is_set() const { return m_p != NULL; }

        /**
          @brief Compares this subbuffer with the first len chars in ptr using char_case.
          @param char_case Defaults to CASE_SENSITIVE.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const char* ptr, size_t len, case_sensitivity_t char_case = CASE_SENSITIVE)
        {
                if (CASE_SENSITIVE == char_case)
                        return m_len == len && std::equal(m_p, m_p + m_len, ptr);
                return m_len == len && 0 == ::strncasecmp(m_p, ptr, m_len);
        }

        /**
          @brief Compares this subbuffer with sb using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const subbuffer& sb, case_sensitivity_t char_case) const
        {
                if (CASE_SENSITIVE == char_case)
                        return m_len == sb.m_len && std::equal(m_p, m_p + m_len, sb.m_p);
                return m_len == sb.m_len && 0 == ::strncasecmp(m_p, sb.m_p, m_len);
        }

        /**
          @brief Compares this subbuffer with sb.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const subbuffer& sb) const
        {
                return m_len == sb.m_len && std::equal(m_p, m_p + m_len, sb.m_p);
        }


        /**
          @brief Compares this subbuffer with sb using char_case.
          Does the comparison starting from the end of the strings to the front.
          So if your strings tend to differ near their ends this could save you some cpu cycles.
          @returns true if they match, false if they don't match-
        	   starting from the last character in the string.
         */
        bool requals(const subbuffer& sb, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (m_len != sb.m_len) return false;

                const char* sb_p  = sb.m_p + sb.m_len - 1;

                if (CASE_SENSITIVE == char_case)
                {
                        for (const char* start = m_p + m_len - 1; start >= m_p; start--, sb_p--)
                        {
                                // comparison order critical here
                                if (*start != *sb_p)
                                        return false;
                        }
                }
                else
                {
                        for (const char* start = m_p + m_len - 1; start >= m_p; start--, sb_p--)
                        {
                                // comparison order critical here
                                if (tolower(*start) != tolower(*sb_p))
                                        return false;
                        }
                }
                return true;
        }

        /**
          @brief Compares this subbuffer with c using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(char c, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (CASE_SENSITIVE == char_case) return 1 == m_len && c == m_p[0];

                return 1 == m_len && tolower(c) == tolower(m_p[0]);
        }

        /**
          @brief Compares this subbuffer with sb.
          @returns an integer less than, equal to, or greater than zero if this subbuffer
        	   is found, respectively, to be less than, to match, or be greater than sb.
         */
        inline int compare(const subbuffer& sb) const
        {
                if (!m_len) return (!sb.m_len) ?  0 : -1;
                if (!sb.m_len) return 1;

                if (*m_p != *sb.m_p) return *m_p - *sb.m_p;
                int ret = ::strncmp(m_p, sb.m_p, std::min(m_len, sb.m_len));
                return (ret ? ret : m_len < sb.m_len ? -1 : m_len > sb.m_len ? 1 : 0);
        }

        /**
          @brief Compares this subbuffer with sb using char_case.
          @returns an integer less than, equal to, or greater than zero if this subbuffer
        	   is found, respectively, to be less than, to match, or be greater than sb.
         */
        inline int compare(const subbuffer& sb, case_sensitivity_t char_case) const
        {
                if (!m_len) return (!sb.m_len) ?  0 : -1;
                if (!sb.m_len) return 1;

                if (CASE_SENSITIVE == char_case)
                {
                        int ret = ::strncmp(m_p, sb.m_p, std::min(m_len, sb.m_len));
                        return (ret ? ret : m_len < sb.m_len ? -1 : m_len > sb.m_len ? 1 : 0);
                }
                int ret = ::strncasecmp(m_p, sb.m_p, std::min(m_len, sb.m_len));
                return (ret ? ret : m_len < sb.m_len ? -1 : m_len > sb.m_len ? 1 : 0);
        }

        /**
          @brief Compares the first sb.length() bytes of this subbuffer.
          @returns true if they match, false if they don't match.
         */
        inline bool starts_with(const subbuffer& sb) const
        {
                return m_len >= sb.m_len && 0 == ::strncmp(m_p, sb.m_p, sb.m_len);
        }

        /**
          @brief Compares the first sb.length() bytes of this subbuffer using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool starts_with(const subbuffer& sb, case_sensitivity_t char_case) const
        {
                if (CASE_SENSITIVE == char_case)
                        return m_len >= sb.m_len && 0 == ::strncmp(m_p, sb.m_p, sb.m_len);
                return m_len >= sb.m_len && 0 == ::strncasecmp(m_p, sb.m_p, sb.m_len);
        }

        /**
          @brief Compares the first byte of this subbuffer using pred().
          The pred object needs to have a "bool operator()(char) const" method
          @returns true if they match, false if they don't match.
         */
        template<class BinaryPredicate> inline bool starts_with(const BinaryPredicate& pred) const
        {
                return m_len && pred(*m_p);
        }

        /**
          @brief Compares the first byte of this subbuffer.
          @returns true if they match, false if they don't match.
         */
        inline bool starts_with(char c) const
        {
                return m_len > 0 && c == m_p[0];
        }

        /**
          @brief Compares the last sb.length() bytes of this subbuffer using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool ends_with(const subbuffer& sb, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (CASE_SENSITIVE == char_case)
                        return m_len >= sb.m_len && 0 == ::strncmp(m_p + (m_len - sb.m_len), sb.m_p, sb.m_len);
                return m_len >= sb.m_len && 0 == ::strncasecmp(m_p + (m_len - sb.m_len), sb.m_p, sb.m_len);
        }

        /**
          @brief Compares the last byte of this subbuffer using cg.in().
          @returns true if they match, false if they don't match.
         */
        inline bool ends_with(const chargrp& cg) const
        {
                return m_len > 0 && cg.contains(m_p[m_len - 1]);
        }

        /**
          @brief Compares the last byte of this subbuffer.
          @returns true if they match, false if they don't match.
         */
        inline bool ends_with(char c) const
        {
                return m_len > 0 && c == m_p[m_len - 1];
        }

        /**
          @brief An array of chars that have all been changed to upper case

          Tried combining this class and caseless_equal_to to pretty it up
          but that added 4 nanoseconds.
         */
        class upper_chars
        {
        public:
                upper_chars()
                {
                        for (int i = CHAR_MIN; i <= CHAR_MAX; ++i)
                                tab[i - CHAR_MIN] = toupper((char) i);

                }
                char tab[CHAR_MAX - CHAR_MIN];
        };

        /**
          @brief A binary functor object used for passing to std::search
          Using an object instead of a function ptr in the str::search method
          shaved 15-20 nanoseconds off the timings.
         */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"  // ignored because binary_function has non-virtual dtor
        struct caseless_equal_to : std::binary_function <int,int,bool>
        {
#pragma GCC diagnostic pop                // back to default behaviour
                caseless_equal_to(char* tab) :
                        m_tab(tab)
                {
                }
                bool operator() (int i, int j) const
                {
                        return m_tab[i - CHAR_MIN] == m_tab[j - CHAR_MIN];
                }
                char* m_tab;
        };

        inline size_t find(const subbuffer& sb) const
        {
                if (!m_len || !sb.m_len || sb.m_len > m_len) return npos;

                const char* p = std::search(m_p, m_p + m_len, sb.m_p, sb.m_p + sb.m_len);
                return (p == m_p + m_len) ? npos : (const char*)p - m_p;
        }

        /**
          @brief Searches for sb in this subbuffer starting at pos and using char_case
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t find(const subbuffer& sb, size_t pos, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (!m_len || !sb.m_len) return npos;
                if (sb.m_len > (m_len - pos) || pos >= m_len) return npos;

                if (CASE_SENSITIVE == char_case)
                {
                        const char* p = std::search(m_p + pos, m_p + m_len, sb.m_p, sb.m_p + sb.m_len);
                        return (p == m_p + m_len) ? npos : (const char*)p - m_p;
                }

                // Decided to leave the following as custom code rather than call std::search with fobj
                // During testing the std::search was bouncing between 38 and 52 nanoseconds.
                // For the same input the following code consistently got 32 nanseconds.

                static upper_chars upchars;
                static caseless_equal_to fobj(upchars.tab);

                const char* str = sb.m_p;
                for (size_t i = pos; i <= m_len - sb.m_len; ++i)
                {
                        if (fobj(m_p[i], *str))
                        {
                                size_t j = 1;
                                for (; j < sb.m_len; ++j)
                                        if (!fobj(m_p[i + j], str[j])) break;
                                if (j >= sb.m_len) return i;
                        }
                }
                return npos;
        }


        /**
          @brief Searches for val in this subbuffer
          @param [in] val The char to search for.
          @returns subbuffer::npos if not found or the the offset if found.
         */

        inline size_t find(char val) const
        {
                if (!m_len) return npos;

                const void* p = memchr(m_p, val, m_len);
                return p ? (const char*)p - m_p : npos;
        }

        /**
          @brief Searches for val in this subbuffer starting at pos and using char_case.
          @param [in] val The char to search for.
          @param [in] pos The index to start the search from. Defaults to 0.
          @param [in] char_case CASE_SENSITIVE (default) or CASE_INSENSITIVE
          @returns subbuffer::npos if not found or the the offset if found.
         */
        inline size_t find(char val, size_t pos, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (!m_len || pos >= m_len) return npos;

                if (CASE_SENSITIVE == char_case)
                {
                        const void* p = memchr(m_p + pos, val, m_len - pos);
                        if (!p) return npos;
                        return (const char*)p - m_p;
                }

                static upper_chars upchars;
                static caseless_equal_to fobj(upchars.tab);
                for (size_t i = pos; i < m_len; ++i)
                {
                        if (fobj(m_p[i], val)) return i;
                }
                return npos;
        }

        /**
          @brief Apply func to all chars starting from pos.
          @param [in] pred Can be either "bool (*pred)(char c)" or an object with "bool operator()(char c)"
         */
        template<class BinaryPredicate> inline bool is_all(BinaryPredicate pred, size_t pos = 0) const
        {
                if (!m_len) return false;

                for (size_t s = pos; s < m_len; ++s)
                        if (!pred(m_p[s])) return false;
                return true;
        }

        /**
          @brief Searches for characters equal to val and returns the number of matches starting from pos.
          @param [in] val The char to count.
          @returns 0 if no matches were found.
         */
        size_t count(char val, size_t pos = 0) const
        {
                if (!m_len) return 0;
                size_t cnt = 0;
                for (size_t s = pos; s < m_len; ++s)
                        if (m_p[s] == val) ++cnt;
                return cnt;
        }

        /**
          @brief Searches for the first char in cg starting at pos.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t find(const chargrp& cg, size_t pos = 0) const
        {
                if (!m_len) return npos;

                for (size_t s = pos; s < m_len; ++s)
                        if (cg.contains(m_p[s])) return s;
                return npos;
        }

        /**
          @brief Searches for sb in this subbuffer, in reverse order.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        inline size_t rfind(char val) const
        {
                if (!m_len) return npos;

                const void* p = memrchr(m_p, val, m_len);
                if (!p) return npos;
                return (const char*)p - m_p;
        }

        /**
          @brief Searches for sb in this subbuffer starting at pos and using char_case.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t rfind(char val, size_t pos, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (!m_len) return npos;
                if (pos >= m_len) pos = m_len - 1;

                if (CASE_SENSITIVE == char_case)
                {
                        for (size_t i = pos; i <= pos; --i)
                                if (m_p[i] == val) return i;
                        return npos;
                }

                static upper_chars upchars;
                static caseless_equal_to fobj(upchars.tab);
                for (size_t i = pos; i <= pos; --i)
                        if (fobj(m_p[i], val)) return i;
                return npos;
        }

        /**
          @brief Searches for the last char in cg starting at pos.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t rfind(const chargrp& cg, size_t pos = npos) const
        {
                if (!m_len) return npos;

                if (pos >= m_len) pos = m_len - 1;

                for (size_t s = pos; s < m_len; --s)
                        if (cg.contains(m_p[s])) return s;
                return npos;
        }

        /**
          @brief Find the first char that is not a member of the given set.
          @returns subbuffer::npos if none or the the offset if found.
         */
        size_t first_not_in(const chargrp& cg, size_t pos = 0) const
        {
                if (empty()) return npos;

                for (size_t s = pos; s < m_len; ++s)
                        if (!cg.contains(m_p[s])) return s;
                return npos;
        }

        /**
          @brief Similar to find except that is is more explicit and easier to understand.
          @returns true if found, false if not found.
         */
        inline bool contains(const subbuffer& sb, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                return npos != find(sb, 0, char_case);
        }


        /**
          @brief Similar to find except that is is more explicit and easier to understand.
          @returns true if found, false if not found.
         */
        inline bool contains(const chargrp& cg) const
        {
                return npos != find(cg);
        }

        /**
          @brief Checks if this subbuffer is contained within the passed in subbuffer.
          @returns true if it is, false if it is not.
         */
        bool is_sub_of(const subbuffer& sb) const
        {
                return (m_p >= sb.m_p && (m_p + m_len) <= (sb.m_p + sb.m_len));
        }

        /**
          @brief Accessor for the char at offset.
          @returns char(0) if subbuffer is empty or offset is out of bounds
         */
        inline char at(size_t offset) const
        {
                if (!m_p || offset >= m_len) return char(0);
                return m_p[offset];
        }

        /**
          @brief Builds/returns a subbuffer from this buffer.
         */
        inline subbuffer sub(size_t pos = 0, size_t len = npos) const
        {
                if (pos >= m_len) return subbuffer(m_p, 0);
                return subbuffer(m_p + pos, std::min(len, m_len - pos));
        }


        /**
          @brief Searches for val.
          @returns The substring before val if found, or the whole subbuffer if not found.
         */
        inline subbuffer before(char val) const
        {
                return sub(0, find(val));
        }

        /**
          @brief Searches for the first char in cg.
          @returns The initial substring if found, or the whole subbuffer if not found.
         */
        inline subbuffer before(const chargrp& cg) const
        {
                return sub(0, find(cg));
        }

        /**
          @brief Searches for the first str.
          @returns The initial substring if found, or the whole subbuffer if not found.
         */
        inline subbuffer before(const subbuffer& str) const
        {
                return sub(0, find(str));
        }

        /**
          @brief Searches for val from the back of the buffer.
          @returns The substring before val if found, or the whole subbuffer if not found.
         */
        inline subbuffer rbefore(char val) const
        {
                return sub(0, rfind(val));
        }

        /**
          @brief Searches for the last char in cg.
          @returns The substring if found, or the whole subbuffer if not found.
         */
        inline subbuffer rbefore(const chargrp& cg) const
        {
                return sub(0, rfind(cg));
        }

        /**
          @brief Searches for val.
          @returns The substring after val if found, or an empty subbuffer if not found.
         */
        inline subbuffer after(char val) const
        {
                const void* p = memchr(m_p, val, m_len);
                if (!p) return subbuffer(0, 0);
                return sub((const char*)p - m_p + 1);
        }

        /**
          @brief Searches for the first char in cg.
          @returns The substring if found, or an empty subbuffer if not found.
         */
        inline subbuffer after(const chargrp& cg) const
        {
                size_t pos = find(cg);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + 1);
        }

        /**
          @brief Searches str.
          @returns The substring if found, or an empty subbuffer if not found.
         */
        inline subbuffer after(const subbuffer& str) const
        {
                size_t pos = find(str);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + str.length());
        }

        /**
          @brief Searches for val from the back of the buffer.
          @returns The substring after val if found, or an empty subbuffer if not found.
         */
        inline subbuffer rafter(char val) const
        {
                size_t pos = rfind(val);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + 1);
        }

        /**
          @brief Searches for the last char in sg.
          @returns The substring if found, or an empty subbuffer if not found.
         */
        inline subbuffer rafter(const chargrp& cg) const
        {
                size_t pos = rfind(cg);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + 1);
        }

        /**
          @brief Removes all occurances of val from the left/front side of the buffer.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& ltrim(char val)
        {
                if (!m_len || *m_p != val) return *this;
                ++m_p;
                --m_len;
                while (m_len && *m_p == val && --m_len)
                {
                        ++m_p;
                }
                return *this;
        }

        inline subbuffer& ltrim(char val, size_t max)
        {
                if (!m_len || *m_p != val) return *this;
                ++m_p;
                --m_len;
                --max;
                while (m_len && *m_p == val && max && --m_len)
                {
                        ++m_p;
                        --max;
                }
                return *this;
        }


        /**
          @brief Removes all chars that isfunc returns true for. Starting from the left/front side of the buffer.
          @returns A reference to this subbuffer.
         */
        subbuffer& ltrim(int (*isfunc)(int))
        {
                if (!m_len || !isfunc(*m_p)) return *this;
                while (m_len && isfunc(*m_p) && --m_len)
                {
                        ++m_p;
                }
                return *this;
        }

        /**
          @brief Removes all chars that pred(char) returns true for. Starting from the left/front side of the buffer.
          @returns A reference to this subbuffer.
         */
        template<class BinaryPredicate> subbuffer& ltrim(const BinaryPredicate& pred)
        {
                if (!m_len || !pred(*m_p)) return *this;
                while (m_len && pred(*m_p) && --m_len)
                {
                        ++m_p;
                }
                return *this;
        }

        /**
          @brief Removes all occurances of val from the right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        subbuffer& rtrim(char val)
        {
                if (!m_len) return *this;
                while (m_len && m_p[m_len - 1] == val)
                        --m_len;
                return *this;
        }

        /**
          @brief Removes all chars that isfunc returns true for. Starting from the right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        subbuffer& rtrim(int (*isfunc)(int))
        {
                if (!m_len) return *this;
                while (m_len && isfunc(m_p[m_len - 1]))
                        --m_len;
                return *this;
        }

        /**
          @brief Removes all chars that pred(char) returns true for. Starting from the right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        template<class BinaryPredicate> inline subbuffer& rtrim(BinaryPredicate pred)
        {
                if (!m_len) return *this;
                while (m_len && pred(m_p[m_len - 1]))
                        --m_len;
                return *this;
        }

        /**
          @brief Removes all occurances of val from the left/front and right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& trim(char val) { return ltrim(val).rtrim(val); }

        /**
          @brief Removes all occurances of chars in cg from the left/front and right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& trim(const chargrp& cg) { return ltrim(cg).rtrim(cg); }

        /**
          @brief Removes all chars from the left/front and right/back side of the buffer while isfunc(char) returns true.
          @returns A reference to this subbuffer
         */
        inline subbuffer& trim(int (*isfunc)(int)) { return ltrim(isfunc).rtrim(isfunc); }

        /**
          @brief Removes all chars from the left/front and right/back side of the buffer while pred(char) returns true.
          @returns A reference to this subbuffer
         */
        template<class BinaryPredicate> inline subbuffer& trim(BinaryPredicate pred) { return ltrim(pred).rtrim(pred); }
        
        /**
          @brief Advances the const char* and reduces the length.
          @returns A reference to this subbuffer.
         */
        subbuffer& advance(size_t bytes)
        {
                if (!m_len) return *this;
                if (bytes >= m_len) return clear();

                m_p += bytes;
                m_len -= bytes;
                return *this;
        }

        /**
          @brief Remove the end of the buffer starting from and including pos.
         */
        subbuffer& remove_from(size_t pos)
        {
                if (pos >= m_len) return *this;
                m_len = pos;
                return *this;
        }

        /**
          @brief Zeroes out the member vars.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& clear()
        {
                m_p = 0;
                m_len = 0;
                return *this;
        }

        /**
          @brief Copy at most len bytes from the memory pointed to by the subbuffer into dest.
         */
        char* copy_to(char* dest, size_t len = subbuffer::npos)
        {
                if (!m_len || !dest || !len) return dest;

                if (len > m_len) len = m_len;
                memcpy(dest, m_p, len);
                dest[len] = 0;
                return dest;
        }

        /**
          @brief Performs an alphanumeric to numeric conversion using the requested base.
          @param [out] val The numeric value to fill.
          @param [out] remainder If the whole subbuffer could not be converted, this is set to the rest.
          @param [in] base Defaults to 10.
         */
        template<typename NUMB> bool aton(NUMB& val, subbuffer* remainder = NULL, uint64_t base = 10) const
        {
                const char* rem = NULL;
                bool ret = aton_internal<NUMB>(val, m_p, m_len, &rem, base);
                // if ret == true then any remainder will be inside of sb
                // else rem will be a NULL terminated error string
                if (remainder) *remainder = ret ? subbuffer(rem, rem ? this->length() - (rem - this->begin()) : 0) : subbuffer(rem);
                return ret;
        }

        inline uint64_t hash(uint64_t res = 0, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                const uint64_t seed = 131; // 31 131 1313 13131 131313 etc..
                const char* buff = m_p;
                if (CASE_SENSITIVE == char_case)
                {
                        for (size_t i = 0; i < m_len; ++i)
                                res = (res * seed) + buff[i];
                }
                else
                {
                        for (size_t i = 0; i < m_len; ++i)
                                res = (res * seed) + tolower(buff[i]);
                }
                res &= 0x7fffffffffffffffUL;
                return res;
        }

        inline bool operator< (const subbuffer& sb) const
        {
                if (!m_len) return sb.m_len ?  true : false;
                if (!sb.m_len) return false;

                if (*m_p != *sb.m_p) return *m_p < *sb.m_p;

                int ret = ::strncmp(m_p, sb.m_p, std::min(m_len, sb.m_len));
                if (ret) return (ret < 0);
                return (m_len < sb.m_len) ? true : false;
        }

        inline bool operator== (const subbuffer& val) const { return equals(val); }

private:
        const char* m_p;
        size_t m_len;
};

/**
  @brief struct for using subbuffer in std collections.
  buffer equals function
  */
struct subbuffer_equals
{
        inline bool operator()(const subbuffer& x, const subbuffer& y) const
        {
                return (x.equals(y));
        }
};

struct subbuffer_hash 
{
        inline size_t operator()(const subbuffer& sb) const
        {
                return sb.hash();
        }
};



#endif
