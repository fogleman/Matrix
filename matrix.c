#include "Python.h"
#include <structmember.h>

// Matrix Functions
void mat_identity(double *m) {
    int i = 0;
    m[i++] = 1; m[i++] = 0; m[i++] = 0; m[i++] = 0;
    m[i++] = 0; m[i++] = 1; m[i++] = 0; m[i++] = 0;
    m[i++] = 0; m[i++] = 0; m[i++] = 1; m[i++] = 0;
    m[i++] = 0; m[i++] = 0; m[i++] = 0; m[i++] = 1;
}

void mat_mat_multiply(double *m, double *a, double *b) {
    int i = 0;
    m[i++] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
    m[i++] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3];
    m[i++] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3];
    m[i++] = a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3];
    m[i++] = a[ 0] * b[ 4] + a[ 4] * b[ 5] + a[ 8] * b[ 6] + a[12] * b[ 7];
    m[i++] = a[ 1] * b[ 4] + a[ 5] * b[ 5] + a[ 9] * b[ 6] + a[13] * b[ 7];
    m[i++] = a[ 2] * b[ 4] + a[ 6] * b[ 5] + a[10] * b[ 6] + a[14] * b[ 7];
    m[i++] = a[ 3] * b[ 4] + a[ 7] * b[ 5] + a[11] * b[ 6] + a[15] * b[ 7];
    m[i++] = a[ 0] * b[ 8] + a[ 4] * b[ 9] + a[ 8] * b[10] + a[12] * b[11];
    m[i++] = a[ 1] * b[ 8] + a[ 5] * b[ 9] + a[ 9] * b[10] + a[13] * b[11];
    m[i++] = a[ 2] * b[ 8] + a[ 6] * b[ 9] + a[10] * b[10] + a[14] * b[11];
    m[i++] = a[ 3] * b[ 8] + a[ 7] * b[ 9] + a[11] * b[10] + a[15] * b[11];
    m[i++] = a[ 0] * b[12] + a[ 4] * b[13] + a[ 8] * b[14] + a[12] * b[15];
    m[i++] = a[ 1] * b[12] + a[ 5] * b[13] + a[ 9] * b[14] + a[13] * b[15];
    m[i++] = a[ 2] * b[12] + a[ 6] * b[13] + a[10] * b[14] + a[14] * b[15];
    m[i++] = a[ 3] * b[12] + a[ 7] * b[13] + a[11] * b[14] + a[15] * b[15];
}

void mat_vec3_multiply(double *v, double *a, double *b) {
    int i = 0;
    v[i++] = a[ 0] * b[0] + a[ 4] * b[1] + a[ 8] * b[2] + a[12];
    v[i++] = a[ 1] * b[0] + a[ 5] * b[1] + a[ 9] * b[2] + a[13];
    v[i++] = a[ 2] * b[0] + a[ 6] * b[1] + a[10] * b[2] + a[14];
}

void mat_vec4_multiply(double *v, double *a, double *b) {
    int i = 0;
    v[i++] = a[ 0] * b[0] + a[ 4] * b[1] + a[ 8] * b[2] + a[12] * b[3];
    v[i++] = a[ 1] * b[0] + a[ 5] * b[1] + a[ 9] * b[2] + a[13] * b[3];
    v[i++] = a[ 2] * b[0] + a[ 6] * b[1] + a[10] * b[2] + a[14] * b[3];
    v[i++] = a[ 3] * b[0] + a[ 7] * b[1] + a[11] * b[2] + a[15] * b[3];
}

// Interface
typedef struct {
    PyObject_HEAD
    double data[16];
} Matrix;

static PyTypeObject MatrixType;

static int matrix_init(Matrix *self, PyObject *args, PyObject *kwargs) {
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
        mat_identity(self->data);
    }
    return 0;
}

static void matrix_dealloc(Matrix *self) {
    self->ob_type->tp_free((PyObject *)self);
}

static PyObject *matrix_value(Matrix *self) {
    double *data = self->data;
    PyObject *result = PyTuple_New(16);
    for (int i = 0; i < 16; i++) {
        PyTuple_SetItem(result, i, Py_BuildValue("f", data[i]));
    }
    return result;
}

static PyObject *matrix_identity(Matrix *self) {
    return PyObject_CallObject((PyObject *)&MatrixType, NULL);
}

static PyObject *matrix_mul(Matrix *self, PyObject *arg) {
    if (PyObject_IsInstance(arg, (PyObject *)&MatrixType)) {
        Matrix *result = (Matrix *)matrix_identity(NULL);
        double *m = result->data;
        double *a = self->data;
        double *b = ((Matrix *)arg)->data;
        mat_mat_multiply(m, a, b);
        return (PyObject *)result;
    }
    return NULL;
}

static PyMemberDef matrix_members[] = {
    {NULL}
};

static PyMethodDef matrix_methods[] = {
    {"value", (PyCFunction)matrix_value, METH_NOARGS, ""},
    {"identity", (PyCFunction)matrix_identity, METH_NOARGS, ""},
    {NULL}
};

static PyNumberMethods matrix_as_number = {
    0,                         //nb_add
    0,                         //nb_subtract
    (binaryfunc)matrix_mul,    //nb_multiply
    0,                         //nb_divide
    0,                         //nb_remainder
    0,                         //nb_divmod
    0,                         //nb_power
    0,                         //nb_negative
    0,                         //nb_positive
    0,                         //nb_absolute
    0,                         //nb_nonzero
    0,                         //nb_invert
    0,                         //nb_lshift
    0,                         //nb_rshift
    0,                         //nb_and
    0,                         //nb_xor
    0,                         //nb_or
    0,                         //nb_coerce
    0,                         //nb_int
    0,                         //nb_long
    0,                         //nb_float
    0,                         //nb_oct
    0,                         //nb_hex
    0,                         //nb_inplace_add
    0,                         //nb_inplace_subtract
    0,                         //nb_inplace_multiply
    0,                         //nb_inplace_divide
    0,                         //nb_inplace_remainder
    0,                         //nb_inplace_power
    0,                         //nb_inplace_lshift
    0,                         //nb_inplace_rshift
    0,                         //nb_inplace_and
    0,                         //nb_inplace_xor
    0,                         //nb_inplace_or
    0,                         //nb_floor_divide
    0,                         //nb_true_divide
    0,                         //nb_inplace_floor_divide
    0,                         //nb_inplace_true_divide
    0,                         //nb_index
};

static PyTypeObject MatrixType = {
    PyObject_HEAD_INIT(NULL)
    0,                         // ob_size
    "Matrix",                  // tp_name
    sizeof(Matrix),            // tp_basicsize
    0,                         // tp_itemsize
    (destructor)matrix_dealloc, // tp_dealloc
    0,                         // tp_print
    0,                         // tp_getattr
    0,                         // tp_setattr
    0,                         // tp_compare
    0,                         // tp_repr
    &matrix_as_number,          // tp_as_number
    0,                         // tp_as_sequence
    0,                         // tp_as_mapping
    0,                         // tp_hash
    0,                         // tp_call
    0,                         // tp_str
    0,                         // tp_getattro
    0,                         // tp_setattro
    0,                         // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flag
    "Matrix object",           // tp_doc
    0,                         // tp_traverse
    0,                         // tp_clear
    0,                         // tp_richcompare
    0,                         // tp_weaklistoffset
    0,                         // tp_iter
    0,                         // tp_iternext
    matrix_methods,            // tp_methods
    matrix_members,            // tp_members
    0,                         // tp_getset
    0,                         // tp_base
    0,                         // tp_dict
    0,                         // tp_descr_get
    0,                         // tp_descr_set
    0,                         // tp_dictoffset
    (initproc)matrix_init,     // tp_init
    0,                         // tp_alloc
    PyType_GenericNew,         // tp_new
};

void initmatrix(void) {
    PyObject *module;
    module = Py_InitModule3("matrix", NULL, "An extension with a type.");
    if (module == NULL) {
        return;
    }
    if (PyType_Ready(&MatrixType) < 0) {
        return;
    }
    Py_INCREF(&MatrixType);
    PyModule_AddObject(module, "Matrix", (PyObject *)&MatrixType);
}
