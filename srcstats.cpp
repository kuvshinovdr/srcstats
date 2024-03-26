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

/// @file   srcstats.cpp
/// @brief  SrcStats console application entry point.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "cpp_stat.hpp"
#include "cpp_decomment.hpp"
#include "file_type.hpp"
#include "file.hpp"
#include "utf8.hpp"

#include <iostream>
#include <chrono>
#include <ranges>
#include <functional>

using namespace std;
namespace fs = filesystem;


namespace srcstats
{

  /// @brief      Check command line arguments for help markers and print help if requested.
  /// @param argc how many command line arguments do we have including the command itself
  /// @param argv the array of the arguments
  /// @return     true if help has been printed, false otherwise
  bool print_help(int argc, char* argv[])
  {
    static string_view const help_markers[]
    {
      "-H"sv,
      "-h"sv,
      "--help"sv,
      "/?"sv,
      "-?"sv,
      "?"sv,
    };

    if (argc == 1 || (argc == 2 && ranges::contains(help_markers, argv[1])))
    {
      cout <<
        "SrcStats v.0.4\n"
        "==============\n\n"
        "Author: Kuvshinov D.R.\n"
        "This has been an educational project for my C++ lessons.\n\n"
        "Pass file or directory paths as command line parameters in order to calculate\n"
        "C++ source files statistics.\n\n"
        "Currently only ASCII encoding is correctly handled.\n\n";

      return true;
    }

    return false;
  }


  /// @brief        Try to perform the action and deal with possible exceptions: std::exception or File_error.
  /// @param action the functional object to be called in exception catching environment
  /// @return       0 if no exceptions have been thrown, 1 otherwise (e.g. to serve as a return code from main)
  int run_and_report_exception(auto action) noexcept
  {
    try
    {
      action();
      return 0;
    }
    catch (File_error const& fe)
    {
      clog << "File error with " << fe.file_path() 
           << ": " << fe.what()  << " (" << fe.file_data() << ')';
    }
    catch (exception const& e)
    {
      clog << "Error: " << e.what();
    }
    catch (...)
    {
      clog << "Unknown exception type.";
    }

    clog << endl;
    return 1;
  }


  /// @brief SrcStats logic.
  class Source_statistics_application
  {
  public:
    int run(int argc, char* argv[])
    {
      return run_and_report_exception([=, this]
        {
          // Check if help is requested.
          if (print_help(argc, argv))
            return;

          auto const start_time = chrono::steady_clock::now();

          // Deal with the arguments.
          for (int i = 1; i < argc; ++i)
            run_and_report_exception(
              bind(mem_fn(&Source_statistics_application::_process_argument), this, argv[i]));

          auto const time_elapsed = chrono::steady_clock::now() - start_time;
          _print_stats();
          cout << "Time elapsed: " << chrono::duration<double>(time_elapsed).count() << "s\n";
        });
    }


  private:
    File_type_recognizer _file_type_recognizer;
    Cpp_statistics       _raw_cpp, _dec_cpp; 
    // _dec_cpp accumulate statistics for source files
    // after comments and empty lines have been removed.

    void _print_stats()
    {
      using std::cout;
      cout << "\n========================\n"
                "# C++ files statistics #\n"
                "========================\n\n";

      if (_raw_cpp.header().files().count() != 0 || _raw_cpp.source().files().count() != 0)
      {
        cout << "Raw files\n"
                "=========\n\n";
        _raw_cpp.print(cout);

        cout << "Decommented files\n"
                "=================\n\n";
        _dec_cpp.print(cout);
      }
      else
      {
        cout << "No header or source files have been found.\n";
      }

      cout << endl;
    }

    
    void _process_file_or_ignore(fs::path const& filename)
    {
      auto const file_type = _file_type_recognizer(filename);
      if (file_type.is_recognized() && file_type.lang() == "C++"sv)
        _process_file(filename, file_type.subtype());
    }


    void _process_file_or_throw(fs::path const& filename)
    {
      auto const file_type = _file_type_recognizer(filename);
      if (!file_type.is_recognized() || file_type.lang() != "C++"sv)
        throw File_error("file is ignored as its extension is of neither header or source", filename);

      _process_file(filename, file_type.subtype());
    }


    void _process_file(fs::path const& filename, string_view file_type)
    {
      constexpr size_t padding_bytes     = 2;
      constexpr size_t maximal_file_size = size_t(10) << 20;

      auto file_data = read_file_to_memory(filename, padding_bytes, maximal_file_size);
      normalize(file_data);

      auto const ft = file_type == "header"sv? 
                        Cpp_statistics::File_type::header: 
                        Cpp_statistics::File_type::source;

      _raw_cpp(String_view(file_data), ft);

      auto const data = file_data.data();
      file_data.resize(distance(data, 
                        cpp_decomment(file_data, data)));
      
      remove_empty_lines_and_whitespace_endings(file_data);

      _dec_cpp(String_view(file_data), ft);
    }


    void _process_argument(char const* arg)
    {
      if (fs::path path = arg; fs::is_directory(path))
      {
        for (fs::recursive_directory_iterator it(path), end; it != end; ++it)
          run_and_report_exception([this, &entry = *it]
            {
              if (entry.is_regular_file())
                _process_file_or_ignore(entry.path());
            });
      }
      else
      {
        _process_file_or_throw(path);
      }
    }
  };

}

// The entry point.
int main(int argc, char* argv[])
{
  srcstats::Source_statistics_application app;
  return app.run(argc, argv);
}
