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

/// @file   cs_stat.cpp
/// @brief  Statistics accumulator class for C# files implementation.
/// @author D.R.Kuvshinov kuvshinovdr at yandex.ru
#include "cs_stat.hpp"
#include "cs_decomment.hpp"

#include "../../file_type.hpp"
#include "../lang_base.hpp"


namespace srcstats
{

  class Cs_statistics
    : public Lang_base<1>
  {
  public:
    /// @brief Initialize the base object.
    Cs_statistics() = default;

    /// @brief Returns "C#" as the language name. 
    [[nodiscard]] std::string_view language_name() const noexcept override
    {
      return "C#"sv;
    }

    /// @brief Register all file types corresponding to C++. 
    void register_file_types(File_type_dispatcher& ftd) override
    {
      for (auto ext : { ".cs"sv, ".csx"sv })
        ftd.register_file_type(ext, this);
    }

    /// @brief Remove comments in place.
    void decomment_in_place(String& file_contents, int = 0) override
    {
      Cs_decomment decomment(file_contents);

      auto const new_size =
        decomment.to(file_contents.data()) - file_contents.data();

      file_contents.resize(new_size);
    }
  };


  Lang_interface_uptr new_cs_statistics()
  {
    return std::make_unique<Cs_statistics>();
  }

}
