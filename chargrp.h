//: ----------------------------------------------------------------------------
//: Copyright (C) 2014 Verizon.  All Rights Reserved.
//:
//: \file:    chargrp.h
//: \details: A binary predicate for checking if a char is in a group of chars 
//: \author:  Donnevan "Scott" Yeager
//: \date:    06/07/2013
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

#ifndef _CHARGRP_H_
#define _CHARGRP_H_

#include <stdint.h>
#include <string.h>

static uint8_t s_BYTE_VALS[] = {1,2,4,8,16,32,64,128};

/**
  @brief Simple, lightweight class for testing the value of a char.

  By itself this class is not too usefull but when combined with string manipulation
  methods it comes in handy.

  @code
  chargrp spc_dot_slash(" ./");
  const char* path = "    ./helloworld.txt";

  while (spc_dot_slash.contains(*path))
      ++path;

  path should now equal "helloworld.txt"
  @endcode
 */
class chargrp
{
public:
        chargrp()
        {
                clear();
        }

        explicit chargrp(const char* szGrp)
        {
                clear();
                add(szGrp);
        }

        /**
          @brief Clears the interal memory.
          */
        void clear()
        {
                memset(m_bits, 0, 32);
        }

        /**
          @brief Adds the chars in szGrp to the cltn of chars.
          @param [in] szGrp Null terminated series of chars to add.
          */
        void add(const char* szGrp)
        {
                if (!szGrp) return;

                while (*szGrp)
                {
                        uint8_t c(*szGrp);
                        m_bits[c / 8] = (m_bits[c / 8] | s_BYTE_VALS[c % 8]);
                        szGrp++;
                }
        }

        /**
          @brief Adds a single char to the cltn of chars.
          */
        void add(char val) { m_bits[uint8_t(val) / 8] = m_bits[val / 8] | s_BYTE_VALS[uint8_t(val) % 8]; }

        /**
          @brief Removes the chars in szGrp from the cltn.
          */
        void remove(const char* szGrp)
        {
                if (!szGrp) return;

                while (*szGrp)
                {
                        uint8_t c(*szGrp);
                        m_bits[c / 8] = m_bits[c / 8] & ~(s_BYTE_VALS[c % 8]);
                        szGrp++;
                }
        }

        /**
          @brief Removes a single char from the cltn.
          */
        void remove(char val) { m_bits[uint8_t(val) / 8] = m_bits[uint8_t(val) / 8] & ~(s_BYTE_VALS[uint8_t(val) % 8]); }

        /**
          @brief Checks if val is in the cltn.
          */
        inline bool contains(char val) const { return m_bits[uint8_t(val) / 8] & s_BYTE_VALS[uint8_t(val) % 8]; }

        /**
          @brief Checks if val is in the cltn. Useful in methods requiring a predicate.
          */
        inline bool operator()(char val) const { return m_bits[uint8_t(val) / 8] & s_BYTE_VALS[uint8_t(val) % 8]; }

private:
        uint8_t m_bits[32]; //!< One bit for each of the possible single byte values.
};

#endif
