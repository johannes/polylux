#pragma once

#include <array>
#include <dlfcn.h>

#include "polylux/main.h"

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
extern void *function_table_void;

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

typedef napi_value* (*napi_callback)(void *env, void *callback_info);

class return_value_wrapper : public polylux::return_value_wrapper {
  bool operator=(bool value) override { return value; }
  long operator=(long value) override { return value; }
  double operator=(double value) override { return value; }
  std::string_view operator=(std::string_view value) override { return value; }
};

class argument_list_wrapper : public polylux::argument_list_wrapper {
	public:
  size_t count() const override { return 0; };

  bool as_bool(size_t /*offset*/) const override { return false; }
  long as_long(size_t /*offset*/) const override { return 1; }
  double as_double(size_t /*offset*/) const override { return .1; }
  std::string_view as_string(size_t /*offset*/) const override { return {"FOOBAR", 6}; }

  void *raw(size_t /*offset*/) const override { return nullptr; }
};

template <std::size_t I, typename function_table_t>
napi_value *wrapper_function(void * /*env*/, void * /*callback_info*/) {
  function_table_t &ft =
      *reinterpret_cast<function_table_t *>(function_table_void);

  return_value_wrapper return_value{};
  argument_list_wrapper args{};
  ft[I].f(return_value, args);

  return nullptr;
}

template <typename function_table_t> class register_functions {
  void *env;
  napi_value *result;
  const function_table_t &function_table;

public:
  register_functions(void *env, napi_value *result,
                     const function_table_t &function_table)
      : env{env}, result{result}, function_table{function_table} {}

  template <std::size_t I> napi_value *recurse() {
    recurse<I - 1>();

    // TODO yikes, copied from below!
    using napi_get_laste_error_info_t =
        napi_status (*)(void *env, const napi_extended_error_info **result);
    auto napi_get_last_error_info =
        (napi_get_laste_error_info_t)dlsym(nullptr, "napi_get_last_error_info");

    using napi_is_exception_pending_t =
        napi_status (*)(void *env, bool *result);
    auto napi_is_exception_pending = (napi_is_exception_pending_t)dlsym(
        nullptr, "napi_is_exception_pending");

    using napi_throw_error_t =
        napi_status (*)(void *env, const char *code, const char *msg);
    auto napi_throw_error =
        (napi_throw_error_t)dlsym(nullptr, "napi_throw_error");

    using napi_create_object_t =
        napi_status (*)(void *env, napi_value **result);
    auto napi_create_object =
        (napi_create_object_t)dlsym(nullptr, "napi_create_object");

    using napi_create_function_t =
        napi_status (*)(void *env, const char *utf8name, size_t length,
                        napi_callback cb, void *data, napi_value **result);
    auto napi_create_function =
        (napi_create_function_t)dlsym(nullptr, "napi_create_function");

    using napi_set_named_property_t = napi_status (*)(
        void *env, napi_value *object, const char *utf8Name, napi_value *value);
    auto napi_set_named_property =
        (napi_set_named_property_t)dlsym(nullptr, "napi_set_named_property");

    napi_value *exported_function;
    NAPI_CALL(env,
              napi_create_function(env, function_table[I - 1].name, SIZE_MAX,
                                   &wrapper_function<I - 1, function_table_t>,
                                   NULL, &exported_function));

    NAPI_CALL(env,
              napi_set_named_property(env, result, function_table[I - 1].name,
                                      exported_function));

    // TODO error handling!
    return nullptr;
  }

  template <> napi_value* recurse<0>() { return nullptr; }
};



template<typename function_table_t>
napi_value* napi_register_module_v1_cpp(void* env, napi_value* /*exports*/) {
  // TODO build some nice abstraction ...
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

  using napi_create_function_t =
      napi_status (*)(void *env, const char *utf8name, size_t length,
                      napi_callback cb, void *data, napi_value **result);
  auto napi_create_function =
      (napi_create_function_t)dlsym(nullptr, "napi_create_function");

  using napi_set_named_property_t = napi_status (*)(
      void *env, napi_value *object, const char *utf8Name, napi_value *value);
  auto napi_set_named_property =
      (napi_set_named_property_t)dlsym(nullptr, "napi_set_named_property");

  napi_value *result;
  NAPI_CALL(env, napi_create_object(env, &result));

  function_table_t *ft =
      reinterpret_cast<function_table_t *>(function_table_void);

  polylux::entry::nodejs::register_functions<function_table_t>
      register_functions{env, result, *ft};
  register_functions
      .template recurse<std::tuple_size<function_table_t>::value>();

  return result;
}

inline napi_value* (*napi_register_module_v1_cpp_ptr)(void* env, napi_value* exports) = nullptr;

extern "C" inline
napi_value* napi_register_module_v1_externc(void* env, napi_value* exports) {
  return napi_register_module_v1_cpp_ptr(env, exports);
}

static napi_module module = {
    1,       0,       __FILE__, &napi_register_module_v1_externc,
    nullptr, nullptr, nullptr};

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
  namespace nodejs {                                                           \
  inline void *function_table_void = &function_table;                          \
                                                                               \
  extern "C" {                                                                 \
  static void polylux_entry_nodejs(void) __attribute__((constructor));         \
                                                                               \
  static void polylux_entry_nodejs(void) {                                     \
    namespace pen = polylux::entry::nodejs;                                    \
    pen::module.nm_modname = #module_name;                                     \
    pen::napi_register_module_v1_cpp_ptr =                                     \
        pen::napi_register_module_v1_cpp<decltype(function_table)>;            \
                                                                               \
    pen::module_entry();                                                       \
  }                                                                            \
  }                                                                            \
  }                                                                            \
  }                                                                            \
  }

