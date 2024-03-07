// srcstats.cpp
// C++ source files statistics
// D.R.Kuvshinov
import std;

using namespace std;
namespace fs = filesystem;


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

  // Main operation
  bool process_file(fs::path const& filename)
  {
    ifstream file(filename);
    if (!file.is_open())
    {
      clog << "Can't open: " << filename << '\n';
      return false;
    }

    ++_total_files;

    for (string line; getline(file, line); ++_total_lines)
    {
      _total_length += line.length();
      _max_length = max(_max_length, line.length());
    }

    return true;
  }

private:
  size_t _total_files  = 0;
  size_t _total_lines  = 0;
  size_t _total_length = 0;
  size_t _max_length   = 0;
};


/// Accumulates separate statistics for header and source files.
class Statistics
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
  bool process_file(fs::path const& filename)
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

    return false;
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


void print_stats(char const* title, auto const& stats)
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


// Entry point. Command lines parameters are paths to files or directories.
int main(int argc, char* argv[])
{
  Statistics stats;

  for (int i = 1; i < argc; ++i)
  {
    if (fs::path path = argv[i]; fs::is_directory(path))
    {
      for (fs::recursive_directory_iterator it(path), end; it != end; ++it)
        if (it->is_regular_file())
          stats.process_file(it->path());
    }
    else if (!stats.process_file(path))
    {
      clog << "Can't process file: " << path << '\n';
    }
  }

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

  return 0;
}
