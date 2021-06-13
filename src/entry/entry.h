#pragma once

namespace polylux {
struct info {
  const char *name;
  const char *version_str;
};
} // namespace polylux

extern const polylux::info polylux_info;

// TODO: this header should be split, right now it is recursively including itself  via these:
#include "entry_python.h"
#include "entry_php.h"
#include "entry_nodejs.h"

#define POLYLUX_ENTRY(name, version_str, function_table)                       \
  const polylux::info polylux_info{#name, version_str};                        \
  POLYLUX_ENTRY_PYTHON(name, function_table)                                   \
  POLYLUX_ENTRY_PHP(name, function_table)                                      \
  POLYLUX_ENTRY_NODEJS(name, function_table)
