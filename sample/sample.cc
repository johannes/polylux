#include "polylux.h"

#include <iostream>

namespace {
void hello(const polylux::argument_list_wrapper & /*args*/) {
  std::cout << "hello ";
}
void world(const polylux::argument_list_wrapper &args) {
  std::cout << "world (called with " << args.count()
            << " args, first arg as long: " << args[0].as_long() << ")\n";
}

void handle_bool(const polylux::argument_list_wrapper &args) {
  std::cout << "bool value: " << args[0].as_bool() << "\n";
}

void handle_long(const polylux::argument_list_wrapper &args) {
  std::cout << "long value: " << args[0].as_long() << "\n";
}

void handle_double(const polylux::argument_list_wrapper &args) {
  std::cout << "double value: " << args[0].as_double() << "\n";
}

void handle_string(const polylux::argument_list_wrapper &args) {
  std::cout << "string value: " << args[0].as_string() << "\n";
}

polylux::function_table_t<6> function_table{
    polylux::named_function{"hello", hello},
    polylux::named_function{"world", world},

    polylux::named_function{"handle_bool", handle_bool},
    polylux::named_function{"handle_long", handle_long},
    polylux::named_function{"handle_double", handle_double},
    polylux::named_function{"handle_string", handle_string}
};
} // namespace

POLYLUX_ENTRY(polylux_demo, "1.0.0", function_table)
