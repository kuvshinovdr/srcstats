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

/// @file   lang_interface.hpp
/// @brief  Abstract base class describing a programming language.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_LANG_INTERFACE_HPP_INCLUDED
#define SRCSTATS_LANG_INTERFACE_HPP_INCLUDED

#include "../file_stat.hpp"

#include <ostream>
#include <memory>


namespace srcstats
{

  class File_type_dispatcher;

  /// @brief Language statistics abstract interface.
  struct Lang_interface
  {
    virtual ~Lang_interface() {}

    /// @brief Get the programming language name.
    [[nodiscard]] virtual std::string_view language_name() const noexcept = 0;

    /// @brief Register all file types corresponding to this language. 
    virtual void register_file_types(File_type_dispatcher&) = 0;

    /// @brief Accumulate statistics for the next raw source file (with comments).
    virtual void accumulate_raw(String const& file_contents, int subtype = 0) = 0;

    /// @brief Remove comments in place.
    virtual void decomment_in_place(String& file_contents, int subtype = 0) = 0;

    /// @brief Accumulate statistics for decommented and cleaned-up source file.
    virtual void accumulate_decommented(String const& file_contents, int subtype = 0) = 0;

    /// @brief Print full statistics for this language.
    virtual void print(std::ostream&) const = 0;

    /// @brief Get total statistics for this language including comments. 
    [[nodiscard]] virtual File_statistics total_with_comments() const noexcept = 0;

    /// @brief Get total statistics for this language excluding comments. 
    [[nodiscard]] virtual File_statistics total_decommented() const noexcept = 0;
  };


  /// @brief An owning pointer to a statistics object.
  using Lang_interface_uptr = std::unique_ptr<Lang_interface>;

}

#endif//SRCSTATS_LANG_INTERFACE_HPP_INCLUDED
