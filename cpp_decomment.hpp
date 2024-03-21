/// @file   cpp_decomment.hpp
/// @brief  Decomment C++ source file.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_CPP_DECOMMENT_HPP_INCLUDED
#define SRCSTATS_CPP_DECOMMENT_HPP_INCLUDED

#include "basic.hpp"


namespace srcstats
{

  /// @brief A finite state machine implementation that removes all comments from a C++ source.
  class Cpp_decomment
  {
  public:
    /// @brief       Setup the source data (with 2-character NUL padding!).
    /// @param begin the pointer to the first character of the source data
    /// @param end   the pointer after the last character of the source data (end[1] must be defined)
    constexpr Cpp_decomment(Character const* begin, Character const* end) noexcept
      : _cur(begin), _end(end) {}

    /// @brief       Setup the source data (with 2-character NUL padding!).
    /// @param data  the pointer to the first character of the source data
    /// @param size  the size of the source data (data[size + 1] must be defined)
    constexpr Cpp_decomment(Character const* data, size_t size) noexcept
      : Cpp_decomment(data, data + size) {}

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
    Character const* _cur = nullptr;
    Character const* _end = nullptr;
    Character*       _out = nullptr;

    void _run()                             noexcept;
    void _single_line_comment()             noexcept;
    void _multiline_comment()               noexcept;
    void _put_literal(Character terminator) noexcept;
    void _put_raw_literal()                 noexcept;

    [[nodiscard]] bool _has_delim(
        Character const* delim_start, 
        Character const* delim_end
      ) noexcept
    {
      for (auto delim = delim_start; delim != delim_end; ++delim)
        if (_cur >= _end || _put() != *delim)
          return false;

      return true;
    }

    Character _put() noexcept
    {
      return *_out++ = *_cur++;
    }
  };

}

#endif//SRCSTATS_CPP_DECOMMENT_HPP_INCLUDED
