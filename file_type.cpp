/// @file   file_type.cpp
/// @brief  Recognize file type, file_type.hpp implementation.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "file_type.hpp"

#include <initializer_list>
#include <tuple>
#include <algorithm>
#include <ranges>


namespace srcstats
{

  File_type_recognizer::File_type_recognizer()
  {
    _ext.reserve(8);
    _file_type.reserve(8);

    File_type header("C++", "header");
    for (auto ext: { ".h", ".hpp", ".hxx", "ixx" })
      register_file_type(ext, header);

    File_type source("C++", "source");
    for (auto ext: { ".c", ".cc", ".cpp", ".cxx" })
      register_file_type(ext, source);

    _sort();
  }


  void File_type_recognizer::register_file_type(std::filesystem::path ext, File_type file_type)
  {
    _ext.emplace_back(std::move(ext));
    _file_type.push_back(file_type);
    _is_dirty = true;
  }


  void File_type_recognizer::_sort()
  {
    std::ranges::sort(std::views::zip(_ext, _file_type), 
      [](auto&& a, auto&& b)
      {
        return std::get<0>(a).compare(std::get<0>(b)) < 0;
      });

    _is_dirty = false;
  }


  File_type File_type_recognizer::operator()(std::filesystem::path const& filename)
  {
    if (_is_dirty)
      _sort();

    auto it = std::ranges::equal_range(_ext, filename.extension(),
      [](auto const& a, auto const& b)
      {
        return a.compare(b) < 0;
      });

    if (it.empty())
      return {};

    return _file_type.at(it.begin() - _ext.begin());
  }

}
