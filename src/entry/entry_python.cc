#include "entry.h"
#include "entry_python.h"

#include <iostream>

using Py_ssize_t = ssize_t;
using PyMethodDef = void;
struct PyModuleDef_Slot { };
using traverseproc = void *;
using inquiry = void *;
using freefunc = void *;

struct _typeobject;

struct PyObject {
    //_PyObject_HEAD_EXTRA
    Py_ssize_t ob_refcnt;
    struct _typeobject *ob_type;
};

typedef struct PyModuleDef_Base {
  PyObject ob_base; // PyObject_HEAD
  PyObject* (*m_init)(void);
  Py_ssize_t m_index;
  PyObject* m_copy;
} PyModuleDef_Base;


typedef struct PyModuleDef{
  PyModuleDef_Base m_base;
  const char* m_name;
  const char* m_doc;
  Py_ssize_t m_size;
  PyMethodDef *m_methods;
  struct PyModuleDef_Slot* m_slots;
  traverseproc m_traverse;
  inquiry m_clear;
  freefunc m_free;
} PyModuleDef;

using Py_ssize_t = ssize_t;

extern "C" {
PyObject *PyModule_Create2(struct PyModuleDef *, int apiver);

extern Py_ssize_t PyTuple_Size(PyObject *);
extern PyObject* PyTuple_GetItem(PyObject *p, Py_ssize_t pos);

extern long PyLong_AsLong(PyObject *obj);
extern double PyFloat_AsDouble(PyObject *obj);

extern int PyObject_IsTrue(PyObject *);

const char* PyUnicode_AsUTF8AndSize(PyObject *unicode, Py_ssize_t *size);
}

namespace polylux {
namespace entry {
namespace python {

size_t argument_list_wrapper::count() const {
  return PyTuple_Size(argtuple);
};

bool argument_list_wrapper::as_bool(size_t offset) const {
  PyObject *arg = PyTuple_GetItem(argtuple, offset);
  return PyObject_IsTrue(arg);
}

long argument_list_wrapper::as_long(size_t offset) const {
  PyObject * arg = PyTuple_GetItem(argtuple, offset);
  return PyLong_AsLong(arg);
}

double argument_list_wrapper::as_double(size_t offset) const {
  PyObject * arg = PyTuple_GetItem(argtuple, offset);
  return PyFloat_AsDouble(arg);
}

std::string_view argument_list_wrapper::as_string(size_t offset) const {
  PyObject *arg = PyTuple_GetItem(argtuple, offset);

  Py_ssize_t len;
  const char *s = PyUnicode_AsUTF8AndSize(arg, &len);
  return {s, static_cast<size_t>(len)};
}

void *argument_list_wrapper::raw(size_t offset) const {
  PyObject *arg = PyTuple_GetItem(argtuple, offset);
  return arg;
}

static PyModuleDef py_module{};

PyObject *init(PyMethodDef *methods) { 
	py_module.m_size = sizeof(PyModuleDef);
	py_module.m_name = polylux_info.name;
	py_module.m_methods = methods;
	return PyModule_Create2(&py_module, 3); }
}
} // namespace entry
} // namespace polylux
