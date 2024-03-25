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

/// @file   cpp_stat.hpp
/// @brief  Statistics accumulator class for C++ files (separate header and source statistics).
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_CPP_STAT_HPP_INCLUDED
#define SRCSTATS_CPP_STAT_HPP_INCLUDED

#include "file_stat.hpp"


namespace srcstats
{

  /// @brief C++ statistics accumulators: separate header and source file statistics.
  class Cpp_statistics
  {
  public:
    // Getters

    /// @brief Read-only access to header file statistics. 
    [[nodiscard]] constexpr File_statistics const& header() const noexcept
    {
      return _header;
    }

    /// @brief Read-only access to source file statistics.
    [[nodiscard]] constexpr File_statistics const& source() const noexcept
    {
      return _source;
    }

    /// @brief Calculate total file statistics.
    [[nodiscard]] constexpr File_statistics total() const noexcept
    {
      return File_statistics{_header}(_source);
    }


    /// @brief    Print header, source and total statistics.
    /// @param os the destination output stream
    /// @return   os
    std::ostream& print(std::ostream& os) const
    {
      bool const
        has_header = _header.files().count() != 0,
        has_source = _source.files().count() != 0;

      if (has_header)
        _header.print(os, "Header file statistics\n"
                          "----------------------\n");

      if (has_source)
        _source.print(os, "Source file statistics\n"
                          "----------------------\n");

      if (has_header && has_source)
        total().print(os, "Total file statistics\n"
                          "---------------------\n");

      return os << std::endl;
    }


    // Mutators

    /// @brief C++ file type: header or source.
    enum class File_type
    {
      header,
      source,
    };

    /// @brief Process the preconditioned file data according to its type.
    Cpp_statistics& operator()(String_view file_data, File_type file_type) noexcept
    {
      switch (file_type)
      {
      case File_type::header: _header(file_data); break;
      case File_type::source: _source(file_data); break;
      }

      return *this;
    }

  private:
    File_statistics _header, _source;
  };

}

#endif//SRCSTATS_CPP_STAT_HPP_INCLUDED
