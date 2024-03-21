/// @file   file_stat.cpp
/// @brief  File statistics accumulator class implementation.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "file_stat.hpp"

#include <ranges>


namespace srcstats
{

  /// Linefeed character.
  constexpr Character LF = u8'\n';

  File_statistics& File_statistics::operator()(String_view file_data) noexcept
  {
    // I doubt this code compiles to an effective machine code sequence...
    // Anyway it's short and "new" (C++20 ranges).
    size_t line_counter = 0;

    using namespace std::views;
    for (auto line_length : file_data | lazy_split(LF) | transform(std::ranges::distance))
    {
      _lines(line_length);
      ++line_counter;
    }

    _files(line_counter);
    
    return *this;
  }

}
