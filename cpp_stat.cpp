
#include "cpp_stat.hpp"
#include "cpp_decomment.hpp"
#include "file.hpp"


namespace srcstats
{

  std::ostream& Cpp_statistics::Stat::print(std::ostream& os) const
  {
    bool const
      has_header = _header.files().count() != 0,
      has_source = _source.files().count() != 0;

    if (has_header)
      _header.print(os, "Header file statistics\n"
                        "----------------------\n"sv);

    if (has_source)
      _source.print(os, "Source file statistics\n"
                        "----------------------\n"sv);

    if (has_header && has_source)
      total().print(os, "Total file statistics\n"
                        "---------------------\n"sv);

    if (has_header || has_source)
      os << std::endl;

    return os;
  }
 

  void Cpp_statistics::register_file_types(File_type_recognizer& rec) const
  {
    constexpr srcstats::File_type 
          header("C++"sv, "header"sv),
          source("C++"sv, "source"sv);

    for (auto ext : { ".h"sv, ".hpp"sv, ".hxx"sv, "ixx"sv })
      rec.register_file_type(ext, header);
    for (auto ext : { ".c"sv, ".cc"sv, ".cpp"sv, ".cxx"sv })
      rec.register_file_type(ext, source);
  }

  
  void Cpp_statistics::consume(String file_contents, std::string_view sub_type)
  {
    auto const ft = sub_type == "header"sv ?
         Cpp_statistics::File_type::header :
         Cpp_statistics::File_type::source ;

    _raw_cpp(file_contents, ft);

    file_contents.resize(std::distance(
      file_contents.data(), cpp_decomment(file_contents, file_contents.data())));

    remove_empty_lines_and_whitespace_endings(file_contents);

    _dec_cpp(file_contents, ft);
  }


  void Cpp_statistics::print(std::ostream& os) const
  {
    if (_raw_cpp.header().files().count() + _raw_cpp.source().files().count() == 0)
      return;

    os << "\n========================\n"
            "# C++ files statistics #\n"
            "========================\n\n";

    os << "Raw files\n"
          "=========\n\n";
    _raw_cpp.print(os);

    os << "Decommented files\n"
          "=================\n\n";
    _dec_cpp.print(os);
    
    os << std::endl;
  }

}
