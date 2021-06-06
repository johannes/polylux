#pragma once

#include "entry.h"

#include <cstdint>

struct zend_module_entry;

namespace polylux {
namespace entry {
namespace php {
typedef void(/*__attribute__((fastcall))*/ *zif_handler)(
    /*zend_execute_data*/ void *execute_data,
    /*zval*/ void *return_value);

struct zend_function_entry {
  const char *fname;
  zif_handler handler;
  const /*struct _zend_internal_arg_info*/ void *arg_info;
  uint32_t num_args;
  uint32_t flags;
};

zend_module_entry *get_module(zend_function_entry *function_table);

extern zend_function_entry* fill_function_table();
extern void *function_table_void;

namespace {
template <std::size_t I, typename function_table_t>
void wrapper_function(void * /*execute_data*/, void * /*return_value*/) {
  function_table_t *ft =
      reinterpret_cast<function_table_t *>(function_table_void);
  (*ft)[I].f();
}

template <typename function_table_t> class fill_table {
  zend_function_entry *php_function_table;
  const function_table_t &function_table;

public:
  fill_table(zend_function_entry *php_function_table,
             const function_table_t &function_table)
      : php_function_table{php_function_table}, function_table{function_table} {
  }

  template <std::size_t I> void recurse() {
    recurse<I - 1>();
    php_function_table[I - 1] = zend_function_entry{
        function_table[I - 1].name, &wrapper_function<I - 1, function_table_t>,
        nullptr, 0, 0};
  }
  template <> void recurse<0>() {}
};
}

} // namespace php
} // namespace entry
} // namespace polylux

#define POLYLUX_ENTRY_PHP(module_name, function_table)                         \
  namespace polylux {                                                          \
  namespace entry {                                                            \
  namespace php {                                                              \
  inline void *function_table_void = &function_table;                          \
  static zend_function_entry                                                   \
      php_function_table[std::tuple_size<decltype(function_table)>::value +    \
                         1] = {};                                              \
  zend_function_entry *fill_function_table() {                                 \
    polylux::entry::php::fill_table<decltype(function_table)> fill{            \
        php_function_table, function_table};                                   \
    fill.recurse<std::tuple_size<decltype(function_table)>::value>();          \
    return php_function_table;                                                 \
  }                                                                            \
  }                                                                            \
  }                                                                            \
  }                                                                            \
                                                                               \
  extern "C" __attribute__((visibility("default"))) zend_module_entry *        \
  get_module(void) {                                                           \
    return polylux::entry::php::get_module(                                    \
        polylux::entry::php::fill_function_table());                           \
  }\

