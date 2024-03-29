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

/// @file   cpp_stat.cpp
/// @brief  Statistics accumulator class for C++ files implementation.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#include "cpp_stat.hpp"
#include "cpp_decomment.hpp"
#include "file.hpp"
#include "file_type.hpp"


namespace srcstats
{

  std::ostream& Cpp_statistics::Stat::print(std::ostream& os) const
  {
    bool const
      has_header = _header.files().count() != 0,
      has_source = _source.files().count() != 0;

    if (has_header)
      _header.print(os, "Header file statistics\n"
                        "----------------------\n"sv);

    if (has_source)
      _source.print(os, "Source file statistics\n"
                        "----------------------\n"sv);

    if (has_header && has_source)
      total().print(os, "Total file statistics\n"
                        "---------------------\n"sv);

    if (has_header || has_source)
      os << std::endl;

    return os;
  }
 

  void Cpp_statistics::register_file_types(File_type_dispatcher& rec)
  {
    for (auto ext : { ".h"sv, ".hpp"sv, ".hxx"sv, "ixx"sv })
      rec.register_file_type(ext, this, fst_header);

    for (auto ext : { ".c"sv, ".cc"sv, ".cpp"sv, ".cxx"sv })
      rec.register_file_type(ext, this, fst_source);
  }

  
  void Cpp_statistics::consume(String file_contents, int subtype)
  {
    _raw_cpp(file_contents, subtype);

    file_contents.resize(
      cpp_decomment(file_contents, file_contents.data()) - file_contents.data());

    remove_empty_lines_and_whitespace_endings(file_contents);

    _dec_cpp(file_contents, subtype);
  }


  void Cpp_statistics::print(std::ostream& os) const
  {
    if (_raw_cpp.header().files().count() + _raw_cpp.source().files().count() == 0)
      return;

    os << "\n========================\n"
            "# C++ files statistics #\n"
            "========================\n\n";

    os << "Raw files\n"
          "=========\n\n";
    _raw_cpp.print(os);

    os << "Decommented files\n"
          "=================\n\n";
    _dec_cpp.print(os);
    
    os << std::endl;
  }

}
