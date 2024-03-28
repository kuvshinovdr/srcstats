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

#include "lang_interface.hpp"


namespace srcstats
{
  
  /// @brief C++ statistics accumulator interface.
  class Cpp_statistics 
    : public Language_statistics_interface
  {
  public:
    /// @brief Register all file types corresponding to C++. 
    void register_file_types(File_type_recognizer&) const override;

    /// @brief Accumulate statistics for the next file that has been transcoded.
    void consume(String file_contents, std::string_view sub_type = ""sv) override;

    /// @brief Print full statistics for C++.
    void print(std::ostream&) const override;

    /// @brief Get total statistics for C++ including comments. 
    [[nodiscard]] File_statistics total_with_comments() const noexcept override
    {
      return _raw_cpp.total();
    }

    /// @brief Get total statistics for C++ excluding comments. 
    [[nodiscard]] File_statistics total_decommented() const noexcept override
    {
      return _dec_cpp.total();
    }

  private:
    /// @brief C++ file type: header or source.
    enum class File_type
    {
      header,
      source,
    };

    /// @brief C++ statistics accumulators: separate header and source file statistics.
    class Stat
    {
    public:
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
        return File_statistics{ _header }(_source);
      }

      /// @brief    Print header, source and total statistics.
      /// @param os the destination output stream
      /// @return   os
      std::ostream& print(std::ostream& os) const;

      /// @brief Process the preconditioned file data according to its type.
      Stat& operator()(String_view file_data, File_type file_type) noexcept
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

    Stat _raw_cpp, _dec_cpp;
  };

}

#endif//SRCSTATS_CPP_STAT_HPP_INCLUDED
