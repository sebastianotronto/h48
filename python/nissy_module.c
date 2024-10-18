#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>

#include "../src/nissy.h"

#define MAX_CUBE_STR_LEN 1024 /* Update when adding formats */

static bool
check_error(long long err)
{
	char err_string[255];

	switch (err) {
	case NISSY_OK: /* Fallthrough */
	case NISSY_WARNING_UNSOLVABLE:
		return true;
	case NISSY_ERROR_INVALID_CUBE:
	case NISSY_ERROR_UNSOLVABLE_CUBE: /* Fallthrough */
	case NISSY_ERROR_INVALID_MOVES:
	case NISSY_ERROR_INVALID_TRANS:
	case NISSY_ERROR_INVALID_FORMAT:
	case NISSY_ERROR_INVALID_SOLVER:
	case NISSY_ERROR_NULL_POINTER:
	case NISSY_ERROR_BUFFER_SIZE:
	case NISSY_ERROR_DATA:
	case NISSY_ERROR_OPTIONS:
	case NISSY_ERROR_UNKNOWN:
	default:
		sprintf(err_string, "Error from libnissy (%lld)", err);
		PyErr_SetString(PyExc_Exception, err_string);
		return false;
	}

}

static PyObject *
string_result(long long err, const char *result)
{
	return check_error(err) ? PyUnicode_FromString(result) : NULL;
}

static PyObject *
string_result_free(long long err, char *result)
{
	PyObject *ret;

	ret = PyUnicode_FromString(result);
	free(result);

	return check_error(err) ? ret : NULL;
}

static PyObject *
long_result(long long result)
{
	check_error(result);
	return PyLong_FromLong(result);
}

static PyObject *
compose(PyObject *self, PyObject *args)
{
	long long err;
	const char *cube, *permutation;
	char result[NISSY_SIZE_B32];

	if (!PyArg_ParseTuple(args, "ss", &cube, &permutation))
		return NULL;

	err = nissy_compose(cube, permutation, result);
	return string_result(err, result);
}

static PyObject *
inverse(PyObject *self, PyObject *args)
{
	long long err;
	const char *cube;
	char result[NISSY_SIZE_B32];

	if (!PyArg_ParseTuple(args, "s", &cube))
		return NULL;

	err = nissy_inverse(cube, result);
	return string_result(err, result);
}

static PyObject *
applymoves(PyObject *self, PyObject *args)
{
	long long err;
	const char *cube, *moves;
	char result[NISSY_SIZE_B32];

	if (!PyArg_ParseTuple(args, "ss", &cube, &moves))
		return NULL;

	err = nissy_applymoves(cube, moves, result);
	return string_result(err, result);
}

static PyObject *
applytrans(PyObject *self, PyObject *args)
{
	long long err;
	const char *cube, *trans;
	char result[NISSY_SIZE_B32];

	if (!PyArg_ParseTuple(args, "ss", &cube, &trans))
		return NULL;

	err = nissy_applytrans(cube, trans, result);
	return string_result(err, result);
}

static PyObject *
convert(PyObject *self, PyObject *args)
{
	long long err;
	const char *fin, *fout, *cube;
	char result[MAX_CUBE_STR_LEN];

	if (!PyArg_ParseTuple(args, "sss", &fin, &fout, &cube))
		return NULL;

	err = nissy_convert(fin, fout, cube, MAX_CUBE_STR_LEN, result);
	return string_result(err, result);
}

static PyObject *
getcube(PyObject *self, PyObject *args)
{
	long long ep, eo, cp, co, err;
	const char *options;
	char result[NISSY_SIZE_B32];

	if (!PyArg_ParseTuple(args, "LLLLs", &ep, &eo, &cp, &co, &options))
		return NULL;

	err = nissy_getcube(ep, eo, cp, co, options, result);
	return string_result(err, result);
}

static PyObject *
datasize(PyObject *self, PyObject *args)
{
	long long result;
	const char *solver;

	if (!PyArg_ParseTuple(args, "s", &solver))
		return NULL;

	result = nissy_datasize(solver);
	return long_result(result);
}

/* TODO: gendata, checkdata and solve and countmoves */

static PyMethodDef nissy_methods[] = {
	{ "compose", compose, METH_VARARGS, NULL },
	{ "inverse", inverse, METH_VARARGS, NULL },
	{ "applymoves", applymoves, METH_VARARGS, NULL },
	{ "applytrans", applytrans, METH_VARARGS, NULL },
	{ "convert", convert, METH_VARARGS, NULL },
	{ "getcube", getcube, METH_VARARGS, NULL },
	{ "datasize", datasize, METH_VARARGS, NULL },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef nissy_python_module = {
	.m_base = PyModuleDef_HEAD_INIT,
	.m_name = "nissy",
	.m_doc = NULL,
	.m_size = -1,
	.m_methods = nissy_methods,
	.m_slots = NULL,
	.m_traverse = NULL,
	.m_clear = NULL,
	.m_free = NULL
};

static void
log_stdout(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
}

PyMODINIT_FUNC PyInit_nissy_python_module(void) {
	PyObject *module;

	nissy_setlogger(log_stdout);
	module = PyModule_Create(&nissy_python_module);
	PyModule_AddStringConstant(module, "solvedcube", NISSY_SOLVED_CUBE);

	return module;
}
