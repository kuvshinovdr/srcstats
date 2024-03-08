// srcstats.cpp
// C++ source files statistics
// D.R.Kuvshinov
import std;

using namespace std;
namespace fs = filesystem;


///////////////////////////////////////////////////////////////////////////////

/// Exception class for file_to_string error reporting.
class File_error
  : public runtime_error
{
public:
  using runtime_error::runtime_error;

  File_error(string const& msg, fs::path const& file_path, uintmax_t file_data = 0)
    : runtime_error(msg),
      _file_path(file_path),
      _file_data(file_data)
  {}

  [[nodiscard]] fs::path const& file_path() const noexcept
  {
    return _file_path;
  }

  [[nodiscard]] uintmax_t file_data() const noexcept
  {
    return _file_data;
  }

private:
  fs::path  _file_path;
  uintmax_t _file_data = 0;
};


/// Try to open a file and read it entire contents into the string. The function throws on errors.
string file_to_string(fs::path const& filename, size_t max_file_size = 100 << 20)
{
  auto const file_size = fs::file_size(filename);
  if (cmp_greater(file_size, max_file_size)) // for a safer comparison of integers of different types.
    throw File_error("file is too big", filename, file_size);

  ifstream file(filename, ios::binary);
  if (!file.is_open())
    throw File_error("failed to open", filename);

  string result(file_size, '\0');
  file.read(result.data(), file_size);
  if (auto const bytes_read = file.gcount(); cmp_not_equal(bytes_read, file_size))
    throw File_error("failed to read", filename, bytes_read);

  return result;
}


///////////////////////////////////////////////////////////////////////////////

/// Generic static interface for "statistics" classes.
template <typename Stats>
concept Statistics =
  default_initializable<Stats> &&
  requires(Stats & stats)
  {
    size_t{ stats.total_files()    };
    size_t{ stats.total_length()   };
    size_t{ stats.total_lines()    };
    size_t{ stats.max_length()     };
    double{ stats.average_lines()  };
    double{ stats.average_length() };
  };
// Actually an overkill in this tiny program.
// It is provided in order to demonstrate concept/requires syntax.


///////////////////////////////////////////////////////////////////////////////

/// Accumulates statistics for a set of files.
class File_statistics
{
public:
  // Getters
  [[nodiscard]] size_t total_files() const noexcept
  {
    return _total_files;
  }

  [[nodiscard]] size_t total_lines() const noexcept
  {
    return _total_lines;
  }

  [[nodiscard]] size_t total_length() const noexcept
  {
    return _total_length;
  }

  [[nodiscard]] size_t max_length() const noexcept
  {
    return _max_length;
  }

  [[nodiscard]] double average_length() const noexcept
  {
    return double(total_length()) / total_lines();
  }

  [[nodiscard]] double average_lines() const noexcept
  {
    return double(total_lines()) / total_files();
  }

  // Main operation, throws.
  void process_file(fs::path const& filename)
  {
    string file_data = file_to_string(filename);
    ++_total_files;

    auto const new_end = remove(file_data.begin(), file_data.end(), '\r');
    file_data.erase(new_end, file_data.end());

    auto const sz = file_data.length();
    for (size_t from = 0, to = min(file_data.find('\n'), sz); from < sz;)
    {
      auto const line_length = to - from;
      _total_length += line_length;
      _max_length = max(_max_length, line_length);

      ++_total_lines;

      from = to + 1;
      to = min(file_data.find('\n', from), sz);
    }
  }

private:
  size_t _total_files  = 0;
  size_t _total_lines  = 0;
  size_t _total_length = 0;
  size_t _max_length   = 0;
};

static_assert(Statistics<File_statistics>);


///////////////////////////////////////////////////////////////////////////////

/// Accumulates separate statistics for header and source files.
class Header_source_statistics
{
public:
  // Getters
  [[nodiscard]] File_statistics const& header() const noexcept
  {
    return _header;
  }

  [[nodiscard]] File_statistics const& source() const noexcept
  {
    return _source;
  }

  [[nodiscard]] size_t total_files() const noexcept
  {
    return header().total_files() + source().total_files();
  }

  [[nodiscard]] size_t total_lines() const noexcept
  {
    return header().total_lines() + source().total_lines();
  }

  [[nodiscard]] size_t total_length() const noexcept
  {
    return header().total_length() + source().total_length();
  }

  [[nodiscard]] size_t max_length() const noexcept
  {
    return max(header().max_length(), source().max_length());
  }

  [[nodiscard]] double average_length() const noexcept
  {
    return double(total_length()) / total_lines();
  }

  [[nodiscard]] double average_lines() const noexcept
  {
    return double(total_lines()) / total_files();
  }

  // Main operation
  void process_file(fs::path const& filename)
  {
    static char const* const header_ext[]
    {
      ".h", ".hpp", ".hxx", ".ixx"
    };

    static char const* const source_ext[]
    {
      ".c", ".cpp", ".cxx", ".cc"
    };

    auto const ext = filename.extension();
    if (_has_ext(ext, header_ext))
      return _header.process_file(filename);
    if (_has_ext(ext, source_ext))
      return _source.process_file(filename);
    // The file is silently ignored if it has not extension from header_ext or source_ext.
  }

private:
  File_statistics _header;
  File_statistics _source;

  [[nodiscard]] static bool _has_ext(fs::path const& ext, auto const& exts)
  {
    for (auto possible_ext : exts)
      if (ext.compare(possible_ext) == 0)
        return true;

    return false;
  }
};

static_assert(Statistics<Header_source_statistics>);


///////////////////////////////////////////////////////////////////////////////

void print_stats(char const* title, Statistics auto const& stats)
{
  cout << title << "\n==============\n";
  cout << "\nTotal files:        " << stats.total_files();
  cout << "\nTotal length:       " << stats.total_length();
  cout << "\nTotal lines:        " << stats.total_lines();
  cout << "\nAvg lines per file: " << stats.average_lines();
  cout << "\nAvg line length:    " << stats.average_length();
  cout << "\nMax line length:    " << stats.max_length();
  cout << "\n\n";
}


/// Try to perform the action and deal with possible exceptions: std::exception or File_error.
void run_and_report_exception(auto action)
{
  try
  {
    action();
  }
  catch (File_error const& fe)
  {
    clog << "Error with " << fe.file_path() << ": " << fe.what() << " (" << fe.file_data() << ")\n";
    clog.flush();
  }
  catch (exception const& e)
  {
    clog << "Error: " << e.what() << endl;
  }
}


// Entry point. Command lines parameters are paths to files or directories.
int main(int argc, char* argv[])
{
  Header_source_statistics stats;

  // Gather statistics.
  auto const start_time = chrono::steady_clock::now();

  for (int i = 1; i < argc; ++i)
  {
    run_and_report_exception([&stats, arg = argv[i]]
      {
        if (fs::path path = arg; fs::is_directory(path))
        {
          for (fs::recursive_directory_iterator it(path), end; it != end; ++it)
            run_and_report_exception([&stats, &entry = *it]
              {
                if (entry.is_regular_file())
                  stats.process_file(entry.path());
              });
        }
        else
        {
          stats.process_file(path);
        }
      });
  }

  auto const time_elapsed = chrono::steady_clock::now() - start_time;

  // Report the gathered statistics.
  bool const
    has_header = stats.header().total_files() != 0,
    has_source = stats.source().total_files() != 0;

  if (has_header && has_source)
    print_stats("Total", stats);
  if (has_header)
    print_stats("Header files", stats.header());
  if (has_source)
    print_stats("Source files", stats.source());
  if (!(has_header || has_source))
    cout << "No header or source files have been found.\n";

  cout << "Time elapsed: " << chrono::hh_mm_ss(time_elapsed) << '\n';

  return 0;
}
