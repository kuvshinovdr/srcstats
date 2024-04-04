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

/// @file   stat_accum.cpp
/// @brief  Statistics accumulator implementation details.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "stat_accum.hpp"

#include <string>


namespace srcstats
{

  void Statistics_accumulator::print(
      std::ostream& os, 
      std::string_view object, 
      std::string_view value
    ) const
  {
    std::string output[5]
    {
      "total ",
      "total ",
      "max ",
      "min ",
      "average "
    };

    output[0] += object;
    for (int i = 1; i < 5; ++i)
      output[i] += value;

    auto const width = srcstats::max(output[0].size(), output[4].size()) + 1;
    for (auto& s: output)
      s.resize(width, ' ');

    os << output[0] << "= " << count()   << '\n';
    os << output[1] << "= " << total()   << '\n';
    os << output[2] << "= " << max()     << '\n';
    os << output[3] << "= " << min()     << '\n';
    os << output[4] << "= " << average() << '\n';
  }

}
