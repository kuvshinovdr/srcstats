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

/// @file   cpp_decomment.cpp
/// @brief  Ad hoc implementation of the Cpp_decomment class.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "cpp_decomment.hpp"


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

    /// @brief       Setup the source data (with 2-character NUL padding!).
    /// @param input input text span
    explicit constexpr Cpp_decomment(String_view input) noexcept
      : Cpp_decomment(input.data(), input.size()) {}

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
    In_ptr _skip_raw_literal()           noexcept;
    In_ptr _skip_single_line_comment()   noexcept;
    In_ptr _skip_multiline_comment()     noexcept;
  };


  ///////////////////////////////////////////////////////////////////
  // Cpp_decomment implementation

  using namespace characters;

  void Cpp_decomment::_run() noexcept
  {
    while (_cur < _end)
    {
      auto const from = _cur, to = _skip_until_comment();
      _out += String_view{from, to}.copy(_out, to - from);
      if (to < _end)
        *_out++ = _comment;
    }
  }


  Cpp_decomment::In_ptr Cpp_decomment::_skip_until_comment() noexcept
  {
    do
    {
      switch (In_ptr const comment_start = _cur; auto const head = *_cur++)
      {
      case slash:
        switch (*_cur++)
        {
        case slash:
          _comment = LF;
          _cur     = _skip_single_line_comment();
          return comment_start;

        case asterisk:
          _comment = space;
          _cur     = _skip_multiline_comment();
          return comment_start;
        }
        break;

      case apos:
      case quote:
        _cur = _skip_literal(head);
        break;

      case R:
        if (*_cur++ == quote)
          _cur = _skip_raw_literal();
        break;
      }
    } while (_cur < _end);

    return _end;
  }


  Cpp_decomment::In_ptr Cpp_decomment::_skip_literal(Character term) noexcept
  {
    auto const end = _end;
    for (auto cur = _cur; cur < end;)
    {
      if (auto const in = *cur++; in == term)
        return cur;
      else
        cur += in == backslash;
    }

    return end;
  }


  Cpp_decomment::In_ptr Cpp_decomment::_skip_raw_literal() noexcept
  {
    String_view       sv { _cur, _end };
    String_view const term = sv.substr(0, sv.find(par_open));

    // Simple case of R"(...)"
    if (term.empty())
    {
      constexpr static Character   token_chars[] { par_close,       quote };
      constexpr static String_view token         { &token_chars[0], 2     };

      if (auto const pos = String_view{ _cur, _end }.find(token); pos != NPOS)
        return _cur + pos + 1;
      return _end;
    }

    // Complex case of R"delim(...)delim"
    for (sv.remove_prefix(min(sv.size(), term.size() + 1)); !sv.empty();)
    {
      auto const pos = sv.find(term);
      if (pos == NPOS)
        break;

      bool const has_close = sv[pos - 1] == par_close;
      sv.remove_prefix(pos + term.size());
      if (has_close && sv.front() == quote)
        return sv.data() + 1;
    }

    return _end;
  }


  Cpp_decomment::In_ptr Cpp_decomment::_skip_single_line_comment() noexcept
  {
    for (String_view sv{ _cur, _end }; !sv.empty();)
    {
      auto const pos = sv.find(LF);
      if (pos == NPOS)
        break;

      bool const finish = sv[pos - 1] != backslash;
      sv.remove_prefix(pos + 1);
      if (finish)
        return sv.data();
    }

    return _end;
  }


  Cpp_decomment::In_ptr Cpp_decomment::_skip_multiline_comment() noexcept
  {
    constexpr static Character   token_chars[] { asterisk,        slash };
    constexpr static String_view token         { &token_chars[0], 2     };

    if (auto const pos = String_view{ _cur, _end }.find(token); pos != NPOS)
      return _cur + pos + 1;
    return _end;
  }


  ///////////////////////////////////////////////////////////////////

  Character* cpp_decomment(String_view input, Character* out)
  {
    return Cpp_decomment{input}.to(out);
  }

}
