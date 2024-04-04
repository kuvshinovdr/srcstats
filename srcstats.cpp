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

#include "file_type.hpp"
#include "file.hpp"
//#include "utf8.hpp" // WIP

#include "langs/cpp/cpp_stat.hpp"

#include <iostream>
#include <chrono>
#include <ranges>
#include <vector>
#include <functional>

using namespace std;
namespace fs = filesystem;


namespace srcstats
{

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


  /// @brief SrcStats application logic.
  class Source_statistics_application
  {
  public:

    Source_statistics_application()
    {
      // Add each supported language here.
      _langs.emplace_back(new_cpp_statistics());

      // Register file types.
      for (auto& lang: _langs)
        lang->register_file_types(_file_type_dispatcher);
    }


    int run(int argc, char* argv[])
    {
      return run_and_report_exception([=, this]
        {
          // Check if help is requested.
          if (_print_help(argc, argv))
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

    File_type_dispatcher             _file_type_dispatcher;
    std::vector<Lang_interface_uptr> _langs;


    /// @brief      Check command line arguments for help markers and print help if requested.
    /// @param argc how many command line arguments do we have including the command itself
    /// @param argv the array of the arguments
    /// @return     true if help has been printed, false otherwise
    bool _print_help(int argc, char* argv[])
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
          "SrcStats v.0.6\n"
          "==============\n\n"
          "Author: Kuvshinov D.R.\n"
          "Pass file or directory paths as command line parameters in order to calculate\n"
          "source files statistics.\n\n"
          "Currently only ASCII encoding is correctly handled.\n\n"
          "Supported input languages: ";

        bool first = true;
        for (auto& lang: _langs)
        {
          if (!first)
            cout << ", ";
          first = false;

          cout << lang->language_name();
        }

        cout << ".\n\n";
        return true;
      }

      return false;
    }


    void _print_stats()
    {
      File_statistics total_raw, total_decommented;

      int active_langs = 0;
      for (auto& lang : _langs)
      {
        auto cur_raw = lang->total_with_comments();
        if (cur_raw.is_empty())
          continue;

        auto cur_dec = lang->total_decommented();

        total_raw(cur_raw);
        total_decommented(cur_dec);

        lang->print(cout);
        ++active_langs;
      }

      switch (active_langs)
      {
      case 0:
        cout << "No source or files have been found.\n";
        [[fallthrough]];
      
      case 1:
        return;

      default:
        cout << "===================================================\n"
                "# Total statistics across all supported languages #\n"
                "===================================================\n\n"sv;

        cout << "Raw files\n"
                "=========\n\n";
        total_raw.print(cout);

        cout << "Decommented files\n"
                "=================\n\n";
        total_decommented.print(cout);

        cout << '\n';
      }
    }


    void _process_argument(char const* arg)
    {
      if (fs::path path = arg; fs::is_directory(path))
      {
        for (fs::recursive_directory_iterator it(path), end; it != end; ++it)
          run_and_report_exception([this, &entry = *it]
            {
              if (entry.is_regular_file())
                _file_type_dispatcher(entry.path());
            });
      }
      else
      {
        if (!_file_type_dispatcher(path))
          throw File_error("file type was not recognized successfully, the file was ignored", path);
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
