#include <iostream>
#include <Python.h>
#include "PyRunner.hpp"

int main(int argc, char* argv[]) {

	Py_Initialize();
	std::string change_dir = "sys.path.append('./scripts')";
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(change_dir.c_str());
	PyObject *pModule = PyImport_ImportModule("testAdd");
	if (!pModule) {
		std::cout << "get module failed." << std::endl;
		return 1;
	}
	PyObject *pFunction = PyObject_GetAttrString(pModule, "add");
	if (!pFunction) {
		std::cout << "get func failed." << std::endl;
		return 1;
	}
	PyObject *args = PyTuple_New(2);
	PyObject *arg1 = PyLong_FromLong(2);
	PyObject *arg2 = PyLong_FromLong(3);
	PyTuple_SetItem(args, 0, arg1);
	PyTuple_SetItem(args, 1, arg2);
	PyObject *pRet = PyObject_CallObject(pFunction, args);
	if (!pRet) {
		std::cout << "call func failed." << std::endl;
		return 1;
	}
	int res = 0;
	PyArg_Parse(pRet,"i", &res);
	std::cout << res << std::endl;
	Py_DecRef(pModule);
	Py_DecRef(pFunction);
	Py_DecRef(args);
	Py_DecRef(arg1);
	Py_DecRef(arg2);
	Py_DecRef(pRet);
	Py_Finalize();

	PyRunner().SimpleString("print('Hello world.')");
	PyRunner runner("testAdd.py");
	std::cout << runner.CallFunc<double>("add")(1.23, 312) << std::endl;

	return 0;
}
