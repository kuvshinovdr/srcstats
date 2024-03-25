/******************************************************************************
MIT License

Copyright (c) 2024 Dmitry R. Kuvshinov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

/// @file   basic.hpp
/// @brief  Basic declarations for the whole SrcStats project.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_BASIC_HPP_INCLUDED
#define SRCSTATS_BASIC_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <string_view>


namespace srcstats
{

  [[nodiscard]] constexpr size_t min(size_t a, size_t b) noexcept
  {
    return a < b ? a : b;
  }

  [[nodiscard]] constexpr size_t max(size_t a, size_t b) noexcept
  {
    return a < b ? b : a;
  }

  /// @brief Local character type.
  using Character = char;

  /// @brief Local string view type.
  using String_view = std::basic_string_view<Character>;

  /// @brief Not-a-position constant.
  constexpr auto NPOS = String_view::npos;

  /// @brief Special character codes.
  namespace characters
  {
    enum : Character
    {
      NUL        = u8'\0',
      TAB        = u8'\t',
      LF         = u8'\n',
      space      = u8' ' ,
      slash      = u8'/' ,
      backslash  = u8'\\',
      asterisk   = u8'*' ,
      apos       = u8'\'',
      quote      = u8'"' ,
      R          = u8'R' ,
      par_open   = u8'(' ,
      par_close  = u8')' ,
    };
  }

}

#endif//SRCSTATS_BASIC_HPP_INCLUDED
