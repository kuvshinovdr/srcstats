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
