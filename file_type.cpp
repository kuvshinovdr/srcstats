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

/// @file   file_type.cpp
/// @brief  Recognize file type, file_type.hpp implementation.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "file_type.hpp"
#include "file.hpp"

#include <initializer_list>
#include <tuple>
#include <algorithm>


namespace srcstats
{

  bool File_type_dispatcher::File_type_desc::operator<
        (File_type_dispatcher::File_type_desc const& other) const
  {
    auto const ext_cmp = ext.compare(other.ext);
    if (ext_cmp < 0)
      return true;
    if (ext_cmp > 0)
      return false;

    if (lang < other.lang)
      return true;
    if (lang > other.lang)
      return false;

    if (subtype < other.subtype)
      return true;
    return false;
  }


  void File_type_dispatcher::_sort()
  {
    std::sort(_desc.begin(), _desc.end());
    _is_dirty = false;
  }


  bool File_type_dispatcher::operator()(std::filesystem::path const& filename)
  {
    if (_is_dirty)
      _sort();

    File_type_desc probe { filename.extension() };

    auto const it = std::upper_bound(_desc.begin(), _desc.end(), probe);

    if (it == _desc.end() || it->ext.compare(probe.ext) != 0)
      return false;

    constexpr size_t padding_bytes     = 16;
    constexpr size_t maximal_file_size = size_t(10) << 20;

    auto file_data = read_file_to_memory(filename, padding_bytes, maximal_file_size);
    normalize(file_data);

    it->lang->consume(std::move(file_data), it->subtype);
    return true;
  }

}
