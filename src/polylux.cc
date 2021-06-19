#include "polylux.h"

namespace polylux {
bool argument_wrapper::as_bool() const { return args.as_bool(offset); }

long argument_wrapper::argument_wrapper::as_long() const {
  return args.as_long(offset);
}

double argument_wrapper::as_double() const { return args.as_double(offset); }

std::string_view argument_wrapper::as_string() const {
  return args.as_string(offset);
}

void *argument_wrapper::raw() const { return args.raw(offset); }
} // namespace polylux
