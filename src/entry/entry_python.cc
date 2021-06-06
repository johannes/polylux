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


extern "C" PyObject * PyModule_Create2(struct PyModuleDef*,
                                     int apiver);


namespace polylux {
namespace entry {
namespace python {

static PyModuleDef py_module{};

PyObject *init(PyMethodDef *methods) { 
	py_module.m_size = sizeof(PyModuleDef);
	py_module.m_name = polylux_info.name;
	py_module.m_methods = methods;
	return PyModule_Create2(&py_module, 3); }
}
} // namespace entry
} // namespace polylux
