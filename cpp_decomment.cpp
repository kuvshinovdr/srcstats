/// @file   cpp_decomment.cpp
/// @brief  Implementation of the Cpp_decomment class.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "cpp_decomment.hpp"


namespace srcstats
{

  using namespace characters;

  void Cpp_decomment::_run() noexcept
  {
    while (_cur < _end)
    {
      switch (auto const head = *_cur++)
      {
      case slash:
        switch (auto const next = *_cur++)
        {
        case slash:
          _single_line_comment();
          *_out++ = LF;
          continue;

        case asterisk:
          _multiline_comment();
          *_out++ = space;
          continue;

        case NUL: // padding
          *_out++ = slash;
          return;

        default:
          *_out++ = slash;
          *_out++ = next;
          continue;
        }
        break;

      case quote:
      case apos:
        *_out++ = head;
        _put_literal(head);
        break;

      case R:
        *_out++ = R;
        {
          auto const next = *_cur++;
          if (next == NUL)
            return;

          *_out++ = next;
          if (next == quote)
            _put_raw_literal();
        }
        break;

      default:
        *_out++ = head;
      }
    }
  }

  
  void Cpp_decomment::_single_line_comment() noexcept
  {
    while (_cur < _end)
    {
      switch (*_cur++)
      {
      case LF:        return;
      case backslash: ++_cur;
      }
    }
  }


  void Cpp_decomment::_multiline_comment() noexcept
  {
    for (; _cur < _end; ++_cur)
    {
      if (_cur[0] == asterisk && _cur[1] == slash)
      {
        _cur += 2;
        return;
      }
    }
  }


  void Cpp_decomment::_put_literal(Character terminator) noexcept
  {
    while (_cur < _end)
    {
      auto const next = *_cur++;
      *_out++ = next;
      if (next == terminator)
        return;

      if (next == backslash)
      {
        if (auto const second = *_cur++)
          *_out++ = second;
        else
          return;
      }
    }
  }


  void Cpp_decomment::_put_raw_literal() noexcept
  {
    // R"(...)"
    // R"delim(...)delim"
    //   ^
    auto const delim_start = _cur; // where the delimiter starts
    while (_cur < _end && *_cur != par_open)
      _put();

    if (_cur == _end)
      return;

    auto const delim_end = _cur; // where is (
    while (_cur < _end)
    {
      if (
           _put() == par_close
        && _has_delim(delim_start, delim_end)
        && _put() == quote
        ) break;
    }
  }

}
