#include "cs_decomment.hpp"


namespace srcstats
{

  using namespace characters;

  void Cs_decomment::_run() noexcept
  {
    while (_cur < _end)
    {
      auto const from = _cur, to = _skip_until_comment();
      _out += String_view{from, to}.copy(_out, to - from);
      if (to < _end)
        *_out++ = _comment;
    }
  }


  Cs_decomment::In_ptr Cs_decomment::_skip_until_comment() noexcept
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

        case quote://I'm not proud of it
          switch(*_cur++){
            case quote:
              switch(*_cur++){
                case quote://Multi-line literal
                  _cur = _skip_multiline_literal();
                  break;
                default://Skipped single-line literal
                
              }
            default: //Single-line literal
              _cur = _skip_literal(head);
              break;
          }
            
        break; 
      
        
      
      }
    } while (_cur < _end);

    return _end;
  }


  Cs_decomment::In_ptr Cs_decomment::_skip_literal(Character term) noexcept
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

  Cs_decomment::In_ptr Cs_decomment::_skip_multiline_literal() noexcept
  { 
   constexpr static Character   token_chars[] { quote, quote, quote };
   constexpr static String_view token         { &token_chars[0], 3  };

   if (auto const pos = String_view{ _cur, _end }.find(token); pos != NPOS)
      return _cur + pos + 1;
    return _end;
  }

  Cs_decomment::In_ptr Cs_decomment::_skip_single_line_comment() noexcept
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


  Cs_decomment::In_ptr Cs_decomment::_skip_multiline_comment() noexcept
  {
    constexpr static Character   token_chars[] { asterisk,        slash };
    constexpr static String_view token         { &token_chars[0], 2     };

    if (auto const pos = String_view{ _cur, _end }.find(token); pos != NPOS)
      return _cur + pos + 1;
    return _end;
  }

  

}
