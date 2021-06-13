#pragma once

#include <dlfcn.h>
#include<stdio.h>

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
            ? "empty error message"                               \
            : error_info->error_message;                          \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)


namespace polylux {
namespace entry {
namespace nodejs {
struct napi_value;


typedef enum {
  napi_ok,
  napi_invalid_arg,
  napi_object_expected,
  napi_string_expected,
  napi_name_expected,
  napi_function_expected,
  napi_number_expected,
  napi_boolean_expected,
  napi_array_expected,
  napi_generic_failure,
  napi_pending_exception,
  napi_cancelled,
  napi_escape_called_twice,
  napi_handle_scope_mismatch,
  napi_callback_scope_mismatch,
  napi_queue_full,
  napi_closing,
  napi_bigint_expected,
  napi_date_expected,
  napi_arraybuffer_expected,
  napi_detachable_arraybuffer_expected,
  napi_would_deadlock,  /* unused */
} napi_status;

struct napi_extended_error_info {
  const char* error_message;
  void* engine_reserved;
  uint32_t engine_error_code;
  napi_status error_code;
};


struct napi_module {
  int nm_version;
  unsigned int nm_flags;
  const char *nm_filename;
  napi_value *(*nm_register_func)(void *, napi_value *);
  const char *nm_modname;
  void *nm_priv;
  void *reserved[4];
};

extern "C" inline napi_value* napi_register_module_v1(void* env, napi_value* exports) {
  using napi_get_laste_error_info_t =
      napi_status (*)(void *env, const napi_extended_error_info **result);
  auto napi_get_last_error_info =
      (napi_get_laste_error_info_t)dlsym(nullptr, "napi_get_last_error_info");

  using napi_is_exception_pending_t = napi_status (*)(void *env, bool *result);
  auto napi_is_exception_pending =
      (napi_is_exception_pending_t)dlsym(nullptr, "napi_is_exception_pending");

  using napi_throw_error_t =
      napi_status (*)(void *env, const char *code, const char *msg);
  auto napi_throw_error = (napi_throw_error_t)dlsym(nullptr, "napi_throw_error");

  using napi_create_object_t = napi_status (*)(void *env, napi_value **result);
  auto napi_create_object = (napi_create_object_t)dlsym(nullptr, "napi_create_object");
  napi_value *result;
  NAPI_CALL(env, napi_create_object(env, &result));
  return result;
}

static napi_module module = {1,       0,      __FILE__, &napi_register_module_v1, nullptr,
                             nullptr, nullptr};

inline void module_entry() {
  auto napi_module_register =
      (void (*)(napi_module *))dlsym(nullptr, "napi_module_register");
  if (!napi_module_register) {
    return;
  }
  napi_module_register(&module);
}

} // namespace nodejs
} // namespace entry
} // namespace polylux

#define POLYLUX_ENTRY_NODEJS(module_name, function_table)                      \
  namespace polylux {                                                          \
  namespace entry {                                                            \
  namespace nodejs {}                                                          \
  }                                                                            \
  }                                                                            \
                                                                               \
  extern "C" {                                                                 \
  static void polylux_entry_nodejs(void) __attribute__((constructor));      \
                                                                               \
  static void polylux_entry_nodejs(void) {                                  \
    polylux::entry::nodejs::module.nm_modname = #module_name;                  \
    polylux::entry::nodejs::module_entry();                                 \
  }                                                                            \
  }

