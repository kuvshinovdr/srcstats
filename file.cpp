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

/// @file   file.cpp
/// @brief  File read and simple transform operations (file.hpp implementation).
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "file.hpp"
#include "basic.hpp"

#include <fstream>
#include <algorithm>


namespace srcstats
{

  using namespace characters;

  File_data read_file_to_memory(
      fs::path const& filename,
      size_t          padding_bytes,
      size_t          max_file_size
    )
  {
    auto const file_size = fs::file_size(filename);
    if (file_size > static_cast<uintmax_t>(max_file_size))
      throw File_error("file is too big", filename, file_size);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
      throw File_error("failed to open", filename);

    File_data result(file_size + padding_bytes, NUL);
    result.resize(file_size);
    
    file.read(result.data(), file_size);
    if (auto const bytes_read = file.gcount(); bytes_read != file_size)
      throw File_error("failed to read", filename, bytes_read);

    return result;
  }


  void normalize(File_data& file_data) noexcept
  {
    std::erase_if(file_data, [](unsigned char ch)
      {
        return ch < space && ch != TAB && ch != LF;
      });
  }


  void remove_empty_lines_and_whitespace_endings(File_data& file_data) noexcept
  {
    auto write_pos = file_data.begin(), end = file_data.end();
    
    for (auto read_pos = write_pos; read_pos != end;)
    {
      // Select next line.
      auto line_begin = read_pos, lf_pos = std::find(read_pos, end, LF), line_end = lf_pos;
      // Remove final spaces.
      while (line_end != line_begin && *(line_end - 1) <= space)
        --line_end;
      // Copy if the line is non-empty.
      if (line_begin != line_end)
        write_pos = std::copy(line_begin, line_end, write_pos);
      // Finish?
      if (lf_pos == end)
        break;

      // Find the beginning of the next line.
      read_pos = std::find_if(lf_pos, end, [](char ch) { return ch > space; });
      if (read_pos != end)
        *write_pos++ = LF;      
    }

    file_data.erase(write_pos, end);
  }

}
