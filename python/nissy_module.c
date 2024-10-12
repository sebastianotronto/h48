#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "../src/nissy.h"

/* TODO from here */

static PyObject *compose(PyObject *self, PyObject *args) {
	const char *cube, *permutation;
	char result[NISSY_SIZE_B32];

	if (!PyArg_ParseTuple(args, "ss", &cube, &permutation))
		return NULL;

	nissy_compose(cube, permutation, result);
	return PyUnicode_FromString(result);
}

static PyMethodDef NissyMethods[] = {
	{ "compose", compose, METH_VARARGS, "Compose two cubes." },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef nissy_python_module = {
	PyModuleDef_HEAD_INIT,
	"nissy",
	NULL,
	-1,
	NissyMethods
};

PyMODINIT_FUNC PyInit_nissy_python_module(void) {
	return PyModule_Create(&nissy_python_module);
}
