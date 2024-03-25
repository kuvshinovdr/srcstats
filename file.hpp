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

/// @file   file.hpp
/// @brief  File read and simple transform operations.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_FILE_HPP_INCLUDED
#define SRCSTATS_FILE_HPP_INCLUDED

#include <filesystem>
#include <string>
#include <stdexcept>


namespace srcstats
{

  /// @brief File contents representation in memory.
  using File_data = std::string;

  namespace fs = std::filesystem;


  /// @brief Exception class for file_to_string error reporting.
  class File_error
    : public std::runtime_error
  {
  public:
    /// @brief           Create a file error exception object.
    /// @param msg       exception message available then by calling what() method
    /// @param file_path the path to the file which caused the exception
    /// @param file_data additional data (file size, successfully read bytes)
    explicit File_error(
        std::string const&   msg       = "file error",
        fs::path const&      file_path = {},
        uintmax_t            file_data = 0
      ) : runtime_error(msg),
          _file_path(file_path),
          _file_data(file_data)
    {}

    /// @brief Read-only access to the stored file path. 
    [[nodiscard]] fs::path const& file_path() const noexcept
    {
      return _file_path;
    }

    /// @brief Read-only access to the stored file size data. 
    [[nodiscard]] uintmax_t file_data() const noexcept
    {
      return _file_data;
    }

  private:
    fs::path  _file_path;
    uintmax_t _file_data;
  };


  /// @brief               Read a file to memory, throw File_error if the file can't be open or read
  /// @param filename      the path to the file to be read
  /// @param padding_bytes how many additional zero bytes should be appended to the end of the file contents (reserved)
  /// @param max_file_size maximal file size possible, throw File_error(file size) if the file is larger
  /// @return              file data object storing file byte content
  File_data read_file_to_memory(
      fs::path const& filename,
      size_t          padding_bytes = 0,
      size_t          max_file_size = ~size_t(0) / 2
    );


  /// @brief           Remove ASCII characters with codes below 32 (SPACE) except for TAB and LF (inplace).
  /// @param file_data File_data object will be resized accordingly
  void normalize(File_data& file_data) noexcept;

  /// @brief           Remove all full-whitespace lines and whitespaces at ends of the lines.
  /// @param file_data File_data object will be resized accordingly
  void remove_empty_lines_and_whitespace_endings(File_data& file_data) noexcept;

}

#endif//SRCSTATS_FILE_HPP_INCLUDED
