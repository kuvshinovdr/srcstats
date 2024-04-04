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

/// @file   file_stat.hpp
/// @brief  File statistics accumulator class: accumulates statistics for files and text lines.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_FILE_STAT_HPP_INCLUDED
#define SRCSTATS_FILE_STAT_HPP_INCLUDED

#include "stat_accum.hpp"

#include <string_view>


namespace srcstats
{

  /// @brief Files (their lengths in text lines) and lines (their lengths in characters) statistics.
  class File_statistics
  {
  public:
    // Getters

    /// @brief Check if our statistics does not have any data.
    [[nodiscard]] constexpr bool is_empty() const noexcept
    {
      return _files.count() == 0;
    }


    /// @brief Access read-only files accumulated statistics. File "value" is this file size in text lines.
    [[nodiscard]] constexpr Statistics_accumulator const& files() const noexcept
    {
      return _files;
    }

    /// @brief Access read-only lines accumulated statistics. Line "value" is this line size in characters.
    [[nodiscard]] constexpr Statistics_accumulator const& lines() const noexcept
    {
      return _lines;
    }


    /// @brief       Print files and lines statistics.
    /// @param os    the destination output stream
    /// @param title a text to be output to os before the statistics
    void print(std::ostream& os, std::string_view title = ""sv) const
    {
      os << title << '\n';
      _files.print(os, "files", "lines");

      os << '\n';
      _lines.print(os, "lines", "characters");
      
      os << '\n';
    }

    
    // Mutators

    /// @brief Accumulate a file presented by a preconditioned string_view (lines are delimited with LF characters).
    File_statistics& operator()(String_view file_data) noexcept;

    /// @brief Update with data from another statistics accumulator. 
    constexpr File_statistics& operator()(File_statistics const& stats) noexcept
    {
      _files(stats.files());
      _lines(stats.lines());
      return *this;
    }

  private:
    Statistics_accumulator _files, _lines;
  };

}

#endif//SRCSTATS_FILE_STAT_HPP_INCLUDED
