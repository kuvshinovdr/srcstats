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

/// @file   file_type.hpp
/// @brief  Recognize file type (currently only by its extension).
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_FILE_TYPE_HPP_INCLUDED
#define SRCSTATS_FILE_TYPE_HPP_INCLUDED

#include "basic.hpp"

#include <vector>
#include <filesystem>


namespace srcstats
{

  /// @brief Represent source file type by its programming language name and possibly file subtype (e.g. header or source).
  class File_type
  {
  public:
    // Constructors

    /// @brief Initialize with empty language that means unrecognized file type.
    constexpr File_type() noexcept = default;

    /// @brief Initialize with selected language and no subtype.
    explicit constexpr File_type(std::string_view lang) noexcept
      : _lang(lang) {}

    /// @brief Initialize with selected language and its subtype.
    constexpr File_type(std::string_view lang, std::string_view subtype) noexcept
      : _lang(lang), _subtype(subtype) {}

    // Getters

    /// @brief Get the selected programming language name. 
    [[nodiscard]] constexpr std::string_view lang() const noexcept
    {
      return _lang;
    }

    /// @brief Get the selected source file subtype designation. 
    [[nodiscard]] constexpr std::string_view subtype() const noexcept
    {
      return _subtype;
    }

    /// @brief Check if any language has been recognized by the code that returned this File_type object. 
    [[nodiscard]] constexpr bool is_recognized() const noexcept
    {
      return !_lang.empty();
    }

  private:
    std::string_view _lang, _subtype;
  };


  /// @brief Given the file path recognize its type.
  class File_type_recognizer
  {
  public:
    // Constructors

    /// @brief By default provide C++ recognition only.
    File_type_recognizer();


    // Accessors

    /// @brief          Try to obtain the file type for the given file. Currently only the file extension is examined.
    /// @param filename path to the file
    /// @return         the found file type (as it was registered before this call) or empty file type if there is none
    [[nodiscard]] File_type operator()(std::filesystem::path const& filename);


    // Mutators

    /// @brief           Add an association between a file extension and a file type (language).
    /// @param ext       file extension
    /// @param file_type file type description (language and subtype)
    void register_file_type(std::filesystem::path ext, File_type file_type);

  private:
    std::vector<std::filesystem::path> _ext;
    std::vector<File_type>             _file_type;
    bool                               _is_dirty   = false;

    void _sort();
  };

}

#endif//SRCSTATS_FILE_TYPE_HPP_INCLUDED
