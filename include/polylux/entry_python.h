#pragma once

#include <cstdint>
#include <dlfcn.h>

#include "polylux/main.h"

struct PyObject;

namespace polylux {
namespace entry {
namespace python {
typedef PyObject *(*PyCFunction)(PyObject *, PyObject *);
struct PyMethodDef {
  const char *name;
  PyCFunction f;
  int flags;
  const char *doc;
};

PyObject *init(PyMethodDef *methods);

class argument_list_wrapper : public polylux::argument_list_wrapper {
  PyObject *argtuple;

public:
  explicit argument_list_wrapper(PyObject *argtuple) : argtuple{argtuple} {}

  size_t count() const override;

  bool as_bool(size_t offset) const override;
  long as_long(size_t offset) const override;
  double as_double(size_t offset) const override;
  std::string_view as_string(size_t offset) const override;

  void *raw(size_t offset) const override;
};

extern void *function_table_void;

template <std::size_t I, typename function_table_t>
PyObject * wrapper_function(PyObject * /*self*/, PyObject * pyargs) {
  function_table_t *ft =
      reinterpret_cast<function_table_t *>(function_table_void);

  argument_list_wrapper args{pyargs};

  (*ft)[I].f(args);
  return (PyObject*)dlsym(nullptr, "_Py_TrueStruct");
}

template <typename function_table_t> class fill_table {
  PyMethodDef *python_function_table;
  const function_table_t &function_table;

public:
  fill_table(PyMethodDef *python_function_table,
             const function_table_t &function_table)
      : python_function_table{python_function_table}, function_table{function_table} {
  }

  template <std::size_t I> void recurse() {
    recurse<I - 1>();
    python_function_table[I - 1] = PyMethodDef{
        function_table[I - 1].name, &wrapper_function<I - 1, function_table_t>,
        0x0001, nullptr};
  }
  template <> void recurse<0>() {}
};
}
} // namespace entry
} // namespace polylux

#define POLYLUX_ENTRY_PYTHON(name, function_table)                                             \
  namespace polylux {                                                          \
  namespace entry {                                                            \
  namespace python {                                                              \
  static  PyMethodDef                                                  \
      python_function_table[std::tuple_size<decltype(function_table)>::value +    \
                         1] = {};                                              \
  inline void *function_table_void = &function_table;                          \
	  \
  PyMethodDef *fill_function_table() {                                 \
    polylux::entry::python::fill_table<decltype(function_table)> fill{            \
        python_function_table, function_table};                                   \
    fill.recurse<std::tuple_size<decltype(function_table)>::value>();          \
    return python_function_table;                                                 \
  }                                                                            \
  }}}\
  extern "C" __attribute__((visibility("default")))                            \
      PyObject *PyInit_##name(void) {                                          \
    return polylux::entry::python::init(polylux::entry::python::fill_function_table());                                     \
  }
