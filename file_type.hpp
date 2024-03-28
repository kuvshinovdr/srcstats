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

#include "lang_interface.hpp"

#include <vector>
#include <filesystem>


namespace srcstats
{

  /// @brief Given the file path dispatch it to the programming language object.
  class File_type_dispatcher
  {
  public:
    /// @brief          Try to obtain the file type for the given file and call the corresponding language object.
    /// Currently only the file extension is examined.
    /// @param filename path to the file
    /// @return         true if the file type was found, false otherwise
    bool operator()(std::filesystem::path const& filename);

    /// @brief           Add an association between a file extension and a file type (language).
    /// @param ext       file extension
    /// @param lang      language object that is to be called for this file type
    /// @param subtype   file subtype (e.g. header or source)
    void register_file_type(std::filesystem::path ext,
            Lang_interface* lang, int subtype = 0)
    {
      _desc.emplace_back(std::move(ext), lang, subtype);
      _is_dirty = true;
    }

  private:

    /// @brief Description of a file type: filename extension, language statistics object, file subtype.
    struct File_type_desc
    {
      std::filesystem::path ext;
      Lang_interface*       lang    = nullptr;
      int                   subtype = 0;

      bool operator<(File_type_desc const&) const;
    };

    std::vector<File_type_desc> _desc;
    bool                        _is_dirty   = false;

    void _sort();
  };

}

#endif//SRCSTATS_FILE_TYPE_HPP_INCLUDED
