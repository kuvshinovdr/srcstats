/// @file   cpp_decomment.cpp
/// @brief  Ad hoc implementation of the Cpp_decomment class.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "cpp_decomment.hpp"


namespace srcstats
{

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

}
