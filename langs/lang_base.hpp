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

/// @file   lang_base.hpp
/// @brief  A templated base class for easier implementation of Lang_interface.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_LANG_BASE_HPP_INCLUDED
#define SRCSTATS_LANG_BASE_HPP_INCLUDED

#include "lang_interface.hpp"

#include <array>
#include <type_traits>
#include <initializer_list>


namespace srcstats
{

  /// @brief File subtype short descriptions (titles).
  template <int SubtypeCount>
  using Subtype_titles = std::array<std::string_view, SubtypeCount>;


  /// @brief Repeat ch len times.
  inline void underline(std::ostream& os, char ch, size_t len) noexcept
  {
    while (len-- != 0)
      os.put(ch);
    os.put('\n');
  }


  /// @brief               Language statistics across all its source file subtypes.
  /// @tparam SubtypeCount how many file subtypes are to be supported
  template <int SubtypeCount>
  class Lang_statistics
  {
  public:
    /// @brief Check if our statistics does not have any data.
    [[nodiscard]] constexpr bool is_empty() const noexcept
    {
      for (auto& stat: _stats)
        if (!stat.is_empty())
          return false;

      return true;
    }

    /// @brief Read-only access to file statistics.
    [[nodiscard]] constexpr File_statistics const& stats(int subtype = 0) const
    {
      return _stats.at(subtype);
    }

    /// @brief Compute the statistics total across all source file subtypes.
    [[nodiscard]] constexpr File_statistics compute_total() const noexcept
    {
      auto result = _stats[0];

      for (int i = 1; i < SubtypeCount; ++i)
        result(_stats[i]);

      return result;
    }

    /// @brief        Print header, source and total statistics.
    /// @param os     the destination output stream
    /// @param titles Titles for file statistics per file subtype.
    void print(std::ostream& os, Subtype_titles<SubtypeCount> const& titles) const
    {
      static constexpr std::string_view suffix      = " file statistics\n"sv;
      static constexpr std::string_view total_title = "Total file statistics\n"
                                                      "---------------------\n"sv;
      if (is_empty())
        return;

      int count = 0;
      for (int i = 0; i < SubtypeCount; ++i)
      {
        if (auto const& stat = _stats[i]; !stat.is_empty())
        {
          if (auto const title = titles[i]; !title.empty())
          {
            os << title << suffix;
            underline(os, '-', title.size() + suffix.size() - 1);
          }

          stat.print(os);
          ++count;
        }
      }

      if (count > 1)
        compute_total().print(os, total_title);
        
      os << std::endl;
    }

    /// @brief Process the preconditioned file data according to its type.
    Lang_statistics& operator()(String_view file_data, int subtype)
    {
      _stats.at(subtype)(file_data);
      return *this;
    }

  private:
    std::array<File_statistics, SubtypeCount> _stats;
  };


  /// @brief This class is needed to provide different constructors based upon SubtypeCount value.
  template <int SubtypeCount>
  class Lang_base_titles
    : public Lang_interface
  {
  protected:
    Lang_base_titles(std::initializer_list<std::string_view> titles_list)
    {
      auto const p = titles_list.begin();
      for (int i = 0; i < SubtypeCount; ++i)
        _titles[i] = p[i];
    }

    [[nodiscard]] constexpr Subtype_titles<SubtypeCount> const& titles() const noexcept
    {
      return _titles;
    }

  private:
    Subtype_titles<SubtypeCount> _titles;
  };

  template <>
  class Lang_base_titles<1>
    : public Lang_interface
  {
  protected:
    Lang_base_titles() {}

    [[nodiscard]] constexpr Subtype_titles<1> const& titles() const noexcept
    {
      return _titles;
    }

  private:
    Subtype_titles<1> _titles;
  };


  /// @brief               Common things for Lang_interface implementations.
  /// @tparam SubtypeCount how many file subtypes are to be supported
  template <int SubtypeCount>
  class Lang_base
    : public Lang_base_titles<SubtypeCount>
  {
  public:
    /// @brief Accumulate statistics for the next raw source file (with comments).
    void accumulate_raw(String const& file_contents, int subtype = 0) override
    {
      _raw(file_contents, subtype);
    }

    /// @brief Accumulate statistics for decommented and cleaned-up source file.
    void accumulate_decommented(String const& file_contents, int subtype = 0) override
    {
      _decommented(file_contents, subtype);
    }

    /// @brief Print full statistics for this language.
    void print(std::ostream& os) const override
    {
      if (_raw.is_empty())
        return;

      static constexpr auto suffix = " statistics #\n"sv;

      auto const lang_name = this->language_name();
      auto const width     = lang_name.size() + suffix.size() + 1;
      
      underline(os, '=', width);
      os << "# " << lang_name << suffix;
      underline(os, '=', width);

      os << "\nRaw files\n"
              "=========\n\n";
      _raw.print(os, titles());

      os << "Decommented files\n"
            "=================\n\n";
      _decommented.print(os, titles());

      os << std::endl;
    }

    /// @brief Get total statistics for this language including comments. 
    [[nodiscard]] File_statistics total_with_comments() const noexcept override
    {
      return _raw.compute_total();
    }

    /// @brief Get total statistics for this language excluding comments. 
    [[nodiscard]] File_statistics total_decommented() const noexcept override
    {
      return _decommented.compute_total();
    }

  protected:
    using Lang_base_titles<SubtypeCount>::Lang_base_titles;
    using Lang_base_titles<SubtypeCount>::titles;

  private:
    Lang_statistics<SubtypeCount> _raw, _decommented;
  };

}

#endif//SRCSTATS_LANG_BASE_HPP_INCLUDED
