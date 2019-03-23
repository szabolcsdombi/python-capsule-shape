#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

PyObject * meth_capsule(PyObject * self, PyObject * args, PyObject * kwargs) {
    static char * keywords[] = {"x", "y", "z", NULL};

    float x, y, z;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "fff", keywords, &x, &y, &z)) {
        return 0;
    }

    Py_RETURN_NONE;
}

PyMethodDef module_methods[] = {
    {"capsule", (PyCFunction)meth_capsule, METH_VARARGS | METH_KEYWORDS, 0},
    {0},
};

PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "capsule_shape", 0, -1, module_methods, 0, 0, 0, 0};

extern "C" PyObject * PyInit_capsule_shape() {
    PyObject * module = PyModule_Create(&module_def);
    return module;
}
