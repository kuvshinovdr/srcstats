/// @file   stat_accum.cpp
/// @brief  Statistics accumulator implementation details.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru

#include "stat_accum.hpp"

#include <string>


namespace srcstats
{

  std::ostream& Statistics_accumulator::print(
      std::ostream& os, 
      std::string_view object, 
      std::string_view value
    ) const
  {
    std::string output[5]
    {
      "total ",
      "total ",
      "max ",
      "min ",
      "average "
    };

    output[0] += object;
    for (int i = 1; i < 5; ++i)
      output[i] += value;

    auto const width = srcstats::max(output[0].size(), output[4].size()) + 1;
    for (auto& s: output)
      s.resize(width, ' ');

    os << output[0] << "= " << count()   << '\n';
    os << output[1] << "= " << total()   << '\n';
    os << output[2] << "= " << max()     << '\n';
    os << output[3] << "= " << min()     << '\n';
    os << output[4] << "= " << average() << '\n';

    return os;
  }

}
