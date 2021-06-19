#include "polylux.h"

#include <iostream>

namespace {
void negate(polylux::return_value_wrapper & return_value,
                 const polylux::argument_list_wrapper &args) {
  return_value = !args[0].as_bool();
}

void square(polylux::return_value_wrapper &return_value,
            const polylux::argument_list_wrapper &args) {
  long value = args[0].as_long();
  return_value = value * value;
}

void half(polylux::return_value_wrapper &return_value,
          const polylux::argument_list_wrapper &args) {
  return_value = args[0].as_double() / 2.;
}

void greet(polylux::return_value_wrapper &return_value,
           const polylux::argument_list_wrapper &args) {
  std::string greeting{"Hello "};
  greeting += args[0].as_string();
  return_value = std::string_view{greeting};
}

polylux::function_table_t<4> function_table{
    polylux::named_function{"negate", negate},
    polylux::named_function{"square", square},
    polylux::named_function{"half", half},
    polylux::named_function{"greet", greet}};
} // namespace

POLYLUX_ENTRY(polylux_demo, "1.0.0", function_table)
