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

/// @file   file_stat.cpp
/// @brief  File statistics accumulator class implementation.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "file_stat.hpp"

#include <ranges>


namespace srcstats
{

  File_statistics& File_statistics::operator()(String_view file_data) noexcept
  {
    // I doubt this code compiles to an effective machine code sequence...
    // Anyway it's short and "new" (C++20 ranges).
    size_t line_counter = 0;

    using namespace std::views;
    for (auto line_length 
              : file_data 
              | lazy_split(characters::LF) 
              | transform(std::ranges::distance))
    {
      _lines(line_length);
      ++line_counter;
    }

    _files(line_counter);
    return *this;
  }

}
