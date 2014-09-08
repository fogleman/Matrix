#include "Python.h"
#include <structmember.h>

typedef struct {
    PyObject_HEAD
    float data[16];
} Matrix;

static PyTypeObject MatrixType;

static int Matrix_init(Matrix *self, PyObject *args, PyObject *kwargs) {
    PyObject *arg = NULL;
    if (!PyArg_ParseTuple(args, "|O", &arg)) {
        return -1;
    }
    if (arg) {
        for (int i = 0; i < 16; i++) {
            PyObject *item = PySequence_GetItem(arg, i);
            if (item == NULL) {
                return -1;
            }
            self->data[i] = PyFloat_AsDouble(item);
        }
    }
    else {
        int i = 0;
        float *d = self->data;
        d[i++] = 1; d[i++] = 0; d[i++] = 0; d[i++] = 0;
        d[i++] = 0; d[i++] = 1; d[i++] = 0; d[i++] = 0;
        d[i++] = 0; d[i++] = 0; d[i++] = 1; d[i++] = 0;
        d[i++] = 0; d[i++] = 0; d[i++] = 0; d[i++] = 1;
    }
    return 0;
}

static void Matrix_dealloc(Matrix *self) {
    self->ob_type->tp_free((PyObject *)self);
}

static PyObject *Matrix_value(Matrix *self) {
    float *data = self->data;
    PyObject *result = PyTuple_New(16);
    for (int i = 0; i < 16; i++) {
        PyTuple_SetItem(result, i, Py_BuildValue("f", data[i]));
    }
    return result;
}

static PyObject *Matrix_identity(Matrix *self) {
    return PyObject_CallObject((PyObject *)&MatrixType, NULL);
}

static PyMemberDef Matrix_members[] = {
    {NULL}
};

static PyMethodDef Matrix_methods[] = {
    {"value", (PyCFunction)Matrix_value, METH_NOARGS, ""},
    {"identity", (PyCFunction)Matrix_identity, METH_NOARGS, ""},
    {NULL}
};

static PyTypeObject MatrixType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size */
    "Matrix",                  /* tp_name */
    sizeof(Matrix),            /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Matrix_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags*/
    "Matrix object",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Matrix_methods,            /* tp_methods */
    Matrix_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Matrix_init,     /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
};

void initmatrix(void) {
    PyObject *module;
    module = Py_InitModule3("matrix", NULL, "An extension with a type.");
    if (module == NULL) {
        return;
    }
    MatrixType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&MatrixType) < 0) {
        return;
    }
    Py_INCREF(&MatrixType);
    PyModule_AddObject(module, "Matrix", (PyObject *)&MatrixType);
}
