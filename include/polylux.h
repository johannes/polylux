#pragma once

#include "polylux/main.h"

#include "polylux/entry_python.h"
#include "polylux/entry_php.h"
#include "polylux/entry_nodejs.h"

#define POLYLUX_ENTRY(name, version_str, function_table)                       \
  const polylux::info polylux_info{#name, version_str};                        \
  POLYLUX_ENTRY_PYTHON(name, function_table)                                   \
  POLYLUX_ENTRY_PHP(name, function_table)                                      \
  POLYLUX_ENTRY_NODEJS(name, function_table)
