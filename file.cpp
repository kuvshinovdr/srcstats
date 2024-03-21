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
    std::erase_if(file_data, [](char ch)
      {
        return ch < space && ch != TAB && ch != LF;
      });
  }


  void remove_empty_lines_and_whitespace_endings(File_data& file_data) noexcept
  {
    auto write_pos = file_data.begin();
    auto read_pos  = file_data.begin();
    auto const end = file_data.end();

    while (read_pos != end)
    {
      switch (*write_pos++ = *read_pos++)
      {
      case LF:
        read_pos = 
          std::find_if(read_pos, end, [](auto ch) { return ch != LF; });
        break;

      case space:
      case TAB:
        if (auto look_ahead = 
                    std::find_if(read_pos, end, [](auto ch) { return ch > space || ch == LF; });
                 look_ahead != end && *look_ahead != LF
           )
        {
          write_pos = std::copy(read_pos, look_ahead, write_pos);
          read_pos  = look_ahead;
        }
        break;

      default:
        break;
      }
    }

    file_data.erase(write_pos, end);
  }

}
