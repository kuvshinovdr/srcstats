/// @file   stat_accum.hpp
/// @brief  Statistics accumulator class.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#ifndef SRCSTATS_STAT_ACCUM_HPP_INCLUDED
#define SRCSTATS_STAT_ACCUM_HPP_INCLUDED

#include "basic.hpp"

#include <ostream>


namespace srcstats
{

  /// @brief Accumulate basic statistics of size_t values (total sum, minimal and maximal values).
  class Statistics_accumulator
  {
  public:
    // Getters

    /// @brief Get how many values have been accumulated.
    [[nodiscard]] constexpr size_t count() const noexcept
    {
      return _count;
    }

    /// @brief Get the minimal accumulated value.
    [[nodiscard]] constexpr size_t min() const noexcept
    {
      return srcstats::min(_min_v, _max_v);
    }

    /// @brief Get the maximal accumulated value.
    [[nodiscard]] constexpr size_t max() const noexcept
    {
      return _max_v;
    }

    /// @brief Get the total sum of the accumulated values. 
    [[nodiscard]] constexpr uintmax_t total() const noexcept
    {
      return _total;
    }

    /// @brief Compute the arithmetic average of all accumulated numbers (NaN if their count is zero).
    [[nodiscard]] constexpr double average() const noexcept
    {
      return static_cast<double>(total()) / count();
    }


    /// @brief        Output statistics to an output stream.
    /// @param os     the destination output stream
    /// @param object what objects were being counted
    /// @param value  what are the values accumulated
    /// @return       os
    std::ostream& print(std::ostream& os, std::string_view object, std::string_view value) const;


    // Mutators
    
    /// @brief Accumulate the next value.
    constexpr Statistics_accumulator& operator()(size_t value) noexcept
    {
      ++_count;
      _min_v = srcstats::min(_min_v, value);
      _max_v = srcstats::max(_max_v, value);
      _total += value;
      return *this;
    }

    /// @brief Update with data from another statistics accumulator.
    constexpr Statistics_accumulator& operator()(Statistics_accumulator const& stats) noexcept
    {
      _count += stats.count();
      _min_v = srcstats::min(_min_v, stats.min());
      _max_v = srcstats::max(_max_v, stats.max());
      _total += stats.total();
      return *this;
    }

  private:
    size_t    _count = 0;
    size_t    _min_v = ~size_t(0);
    size_t    _max_v = 0;
    uintmax_t _total = 0;
  };

}

#endif//SRCSTATS_STAT_ACCUM_HPP_INCLUDED
