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
    : public Lang_interface
  {
  public:
    /// @brief Returns "C++" as the language name. 
    [[nodiscard]] std::string_view language_name() const noexcept override
    {
      return "C++"sv;
    }

    /// @brief Register all file types corresponding to C++. 
    void register_file_types(File_type_dispatcher&) override;

    /// @brief Accumulate statistics for the next raw C++ file (with comments).
    void accumulate_raw(String const& file_contents, int subtype = 0) override;

    /// @brief Remove comments in place.
    void decomment_in_place(String& file_contents, int subtype = 0) override;

    /// @brief Accumulate statistics for decommented and cleaned-up source file.
    void accumulate_decommented(String const& file_contents, int subtype = 0) override;

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

    // File subtypes.
    constexpr static int fst_header = 0;
    constexpr static int fst_source = 1;
    

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
      Stat& operator()(String_view file_data, int subtype) noexcept
      {
        switch (subtype)
        {
        case fst_header: _header(file_data); break;
        case fst_source: _source(file_data); break;
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
