/******************************************************************************
MIT License

Copyright (c) 2024 Nikita E. Voronin

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

/// @file   cs_decomment.cpp
/// @brief  Ad hoc implementation of the Cs_decomment class.
/// @author N.E.Voronin nikitvoronin953 at gmail.com
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
   size_t depth = 3; //How many " is needed to get out of literal
   
   while(*_cur++ == quote) ++depth;

   static Character *token_chars = new Character[depth];
   for(size_t i = 0; i < depth; ++i) token_chars[i] = quote; 
   static String_view token { &token_chars[0], depth  };

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
