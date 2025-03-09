#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>

#include "../src/nissy.h"

#define MAX_CUBE_STR_LEN 1024 /* Update when adding formats */
#define MAX_SOLUTIONS_SIZE 250000

static bool
check_error(long long err)
{
	char err_string[255];

	/* A positive value always denotes a success */
	if (err > 0)
		return true;

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


PyDoc_STRVAR(compose_doc,
"compose(cube, permutation)\n"
"--\n\n"
"Apply 'permutation' on 'cube'.\n"
"\n"
"Parameters:\n"
"  - cube: a cube in B32 format\n"
"  - permutation: another cube in B32 format\n"
"\n"
"Returns: the resulting cube string in B32 format\n"
);
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

PyDoc_STRVAR(inverse_doc,
"inverse(cube)\n"
"--\n\n"
"Invert 'cube'.\n"
"\n"
"Parameters:\n"
"  - cube: a cube in B32 format\n"
"\n"
"Returns: the inverse cube in B32 format\n"
);
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

PyDoc_STRVAR(applymoves_doc,
"applymoves(cube, moves)\n"
"--\n\n"
"Apply 'moves' to 'cube'.\n"
"\n"
"Parameters:\n"
"  - cube: a cube in B32 format\n"
"  - moves: the moves to apply on the cube\n"
"\n"
"Returns: the resulting cube in B32 format\n"
);
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

PyDoc_STRVAR(applytrans_doc,
"applytrans(cube, transformation)\n"
"--\n\n"
"Apply 'transformation' to 'cube'.\n"
"\n"
"Parameters:\n"
"  - cube: a cube in B32 format\n"
"  - transformation: the transformation to apply on the cube, formatted as\n"
"    (rotation|mirrored) (2 letters)\n"
"    for example 'mirrored ur' or 'rotation lf'\n"
"\n"
"Returns: the resulting cube in B32 format\n"
);
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

PyDoc_STRVAR(convert_doc,
"convert(fin, fout, cube)\n"
"--\n\n"
"Convert 'cube' from format 'fin' to format 'fout'.\n"
"\n"
"Parameters:\n"
"  - fin: the format in which 'cube' is given\n"
"  - fout: the format to which 'cube' has to be converted\n"
"  - cube: a cube in B32 format\n"
"\n"
"Returns: 'cube' in 'fout' format\n"
);
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

PyDoc_STRVAR(getcube_doc,
"getcube(ep, eo, cp, co, options)\n"
"--\n\n"
"Constructs the cube from the given coordinates and options\n"
"\n"
"Parameters:\n"
"  - ep: the edge permutation coordinate\n"
"  - eo: the edge orientation coordinate\n"
"  - cp: the corner permutation coordinate\n"
"  - co: the corner orientation coordinate\n"
"  - options: a string, for example \"fix\"\n"
"\n"
"Returns: the cube constructed from the given coordinates\n"
);
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

PyDoc_STRVAR(datasize_doc,
"datasize(solver)\n"
"--\n\n"
"Returns the size of the data (pruning table) for the given solver\n"
"\n"
"Parameters:\n"
"  - solver: the name of the solver\n"
"\n"
"Returns: the size of the data for the solver, in bytes\n"
);
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

PyDoc_STRVAR(gendata_doc,
"gendata(solver)\n"
"--\n\n"
"Generates the data (pruning table) for the given solver\n"
"\n"
"Parameters:\n"
"  - solver: the name of the solver\n"
"\n"
"Returns: a bytearray containing the data for the solver\n"
);
static PyObject *
gendata(PyObject *self, PyObject *args)
{
	long long size, err;
	const char *solver;
	char *buf;

	if (!PyArg_ParseTuple(args, "s", &solver))
		return NULL;

	size = nissy_datasize(solver);
	if (!check_error(size))
		return NULL;

	buf = PyMem_Malloc(size);

	Py_BEGIN_ALLOW_THREADS
	err = nissy_gendata(solver, size, buf);
	Py_END_ALLOW_THREADS

	if (check_error(err))
		return PyByteArray_FromStringAndSize(buf, size);
	else
		return NULL;
}

PyDoc_STRVAR(checkdata_doc,
"checkdata(data)\n"
"--\n\n"
"Checks if the data (pruning table) given is valid or not\n"
"\n"
"Parameters:\n"
"  - data: a bytearray containing the data for a solver"
"\n"
"Returns: true if the data is valid, false otherwise\n"
);
PyObject *
checkdata(PyObject *self, PyObject *args)
{
	long long result;
	PyByteArrayObject *data;

	if (!PyArg_ParseTuple(args, "Y", &data))
		return NULL;

	result = nissy_checkdata(data->ob_alloc, data->ob_bytes);

	if (check_error(result))
		return Py_True;
	else
		return Py_False;
}

PyDoc_STRVAR(solve_doc,
"solve(cube, solver, nissflag, minmoves, maxmoves, maxsolutions,"
" optimal, threads, data)\n"
"--\n\n"
"Solves the given 'cube' with the given 'solver' and other parameters."
"See the documentation for libnissy (in nissy.h) for details.\n"
"\n"
"Parameters:\n"
"  - cube: a cube in B32 format\n"
"  - solver: the solver to use\n"
"  - options: extra options for the chosen solver\n"
"  - minmoves: the minimum number of moves to use\n"
"  - maxmoves: the maximum number of moves to use\n"
"  - maxsolution: the maximum number of solutions to return\n"
"  - optimal: the largest number of moves from the shortest solution"
"(set to -1 to ignore)\n"
"  - threads: the number of threads to use (0 for default)\n"
"  - data: a bytearray containing the data for the solver\n"
"\n"
"Returns: a list with the solutions found\n"
);
PyObject *
solve(PyObject *self, PyObject *args)
{
	long long result;
	unsigned nissflag, minmoves, maxmoves, maxsolutions;
	int optimal, i, j, k, threads;
	const char *cube, *solver, *options;
	char solutions[MAX_SOLUTIONS_SIZE];
	long long stats[NISSY_SIZE_SOLVE_STATS];
	PyByteArrayObject *data;
	PyObject *list, *item;

	if (!PyArg_ParseTuple(args, "sssIIIIiiY", &cube, &solver, &options,
	    &nissflag, &minmoves, &maxmoves, &maxsolutions, &optimal,
	    &threads, &data))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	result = nissy_solve(cube, solver, options, nissflag, minmoves,
	    maxmoves, maxsolutions, optimal, threads, data->ob_alloc,
	    data->ob_bytes, MAX_SOLUTIONS_SIZE, solutions, stats);
	Py_END_ALLOW_THREADS

	if(!check_error(result)) {
		return NULL;
	} else {
		list = PyList_New(result);
		for (i = 0, j = 0, k = 0; solutions[i] != 0; i++) {
			if (solutions[i] != '\n')
				continue;
			solutions[i] = 0;
			item = PyUnicode_FromString(&solutions[k]);
			PyList_SetItem(list, j, item);
			j++;
			k = i+1;
		}
		return list;
	}
}

PyDoc_STRVAR(countmoves_doc,
"countmoves(moves)\n"
"--\n\n"
"Count the moves\n"
"\n"
"Parameters:\n"
"  - moves: the moves to be counted\n"
"\n"
"Returns: the number of moves in HTM metric\n"
);
PyObject *
countmoves(PyObject *self, PyObject *args)
{
	long long count;
	const char *moves;

	if (!PyArg_ParseTuple(args, "s", &moves))
		return NULL;

	count = nissy_countmoves(moves);
	return long_result(count);
}

static PyMethodDef nissy_methods[] = {
	{ "compose", compose, METH_VARARGS, compose_doc },
	{ "inverse", inverse, METH_VARARGS, inverse_doc },
	{ "applymoves", applymoves, METH_VARARGS, applymoves_doc },
	{ "applytrans", applytrans, METH_VARARGS, applytrans_doc },
	{ "convert", convert, METH_VARARGS, convert_doc },
	{ "getcube", getcube, METH_VARARGS, getcube_doc },
	{ "datasize", datasize, METH_VARARGS, datasize_doc },
	{ "gendata", gendata, METH_VARARGS, gendata_doc },
	{ "checkdata", checkdata, METH_VARARGS, checkdata_doc },
	{ "solve", solve, METH_VARARGS, solve_doc },
	{ "countmoves", countmoves, METH_VARARGS, countmoves_doc },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef nissy_python_module = {
	.m_base = PyModuleDef_HEAD_INIT,
	.m_name = "nissy",
	.m_doc = "python module for libnissy",
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

	PyModule_AddStringConstant(module, "solved_cube", NISSY_SOLVED_CUBE);
	PyModule_AddIntConstant(module, "nissflag_normal", NISSY_NISSFLAG_NORMAL);
	PyModule_AddIntConstant(module, "nissflag_inverse", NISSY_NISSFLAG_INVERSE);
	PyModule_AddIntConstant(module, "nissflag_mixed", NISSY_NISSFLAG_MIXED);
	PyModule_AddIntConstant(module, "nissflag_linear", NISSY_NISSFLAG_LINEAR);
	PyModule_AddIntConstant(module, "nissflag_all", NISSY_NISSFLAG_ALL);

	return module;
}
