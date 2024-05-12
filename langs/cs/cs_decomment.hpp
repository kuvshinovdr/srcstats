#ifndef SRCSTATS_CS_DECOMMENT_HPP_INCLUDED
#define SRCSTATS_CS_DECOMMENT_HPP_INCLUDED

#include "../../basic.hpp"


namespace srcstats
{
  
  /// @brief A finite state machine implementation that removes all comments from a C++ source.
  class Cs_decomment
  {
  public:
    /// @brief       Setup the source data (with 2-character NUL padding!).
    /// @param begin the pointer to the first character of the source data
    /// @param end   the pointer after the last character of the source data (end[1] must be defined)
    constexpr Cs_decomment(Character const* begin, Character const* end) noexcept
      : _cur(begin), _end(end) {}

    /// @brief       Setup the source data (with 2-character NUL padding!).
    /// @param data  the pointer to the first character of the source data
    /// @param size  the size of the source data (data[size + 1] must be defined)
    constexpr Cs_decomment(Character const* data, size_t size) noexcept
      : Cs_decomment(data, data + size) {}

    /// @brief       Setup the source data (with 2-character NUL padding!).
    /// @param input input text span
    explicit constexpr Cs_decomment(String_view input) noexcept
      : Cs_decomment(input.data(), input.size()) {}

    /// @brief       Do the job.
    /// @param out   the pointer to the output buffer (must be no less than the source input)
    /// @return      pointer to the end of the written data
    Character* to(Character* out) noexcept
    {
      _out = out;
      _run();
      return _out;
    }

  private:
    using In_ptr = Character const*;
    using Out_ptr = Character*;

    In_ptr    _cur{ nullptr };
    In_ptr    _end{ nullptr };
    Out_ptr   _out{ nullptr };
    Character _comment{}; // a character which is to be output in place of the skipped comment

    void   _run()                        noexcept;
    In_ptr _skip_until_comment()         noexcept; // returns where comment starts, _cur is set just after the comment 
    In_ptr _skip_literal(Character term) noexcept;
    In_ptr _skip_multiline_literal()     noexcept;
    In_ptr _skip_raw_literal()           noexcept;
    In_ptr _skip_single_line_comment()   noexcept;
    In_ptr _skip_multiline_comment()     noexcept;
  };

}

#endif//SRCSTATS_CS_DECOMMENT_HPP_INCLUDED
