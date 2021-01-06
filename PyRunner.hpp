#ifndef _PYRUNNER_HPP_
#define _PYRUNNER_HPP_

#include <Python.h>
#include <type_traits>
#include <iostream>
#include <vector>
#include <regex>

class PyRunner;

template<typename _RetTp>
class PyFunctor {
friend class PyRunner;
private:
	PyFunctor(PyObject *pModule, PyObject *pFunction)
		: pModule(pModule), pFunc(pFunction) {
	}
public:
	~PyFunctor() {
		if (pFunc) {
			Py_DecRef(pFunc);
		}
	}
	PyFunctor(PyFunctor<_RetTp> &_old)
		: pModule(_old.pModule), pFunc(_old.pFunc){
	}
	PyFunctor(PyFunctor<_RetTp> &&_old)
		: pModule(_old.pModule), pFunc(_old.pFunc){
		_old.pFunc = nullptr;
	}
	PyFunctor<_RetTp> &operator=(PyFunctor<_RetTp>&) = delete;
	PyFunctor<_RetTp> &operator=(PyFunctor<_RetTp>&&) = delete;
public:
	template<typename... _Args>
	_RetTp operator()(_Args... args) {
		if (pModule == nullptr || pFunc == nullptr) {
			throw std::runtime_error("module or func not cunzaia");
		}
		auto pArgList = PyTuple_New(sizeof...(args));
		pArgs.resize(0);
		pArgs.resize(sizeof...(args), nullptr);
		SetArgList(pArgList, args...);
		auto pRet = PyObject_CallObject(pFunc, pArgList);
		for (auto &pArg : pArgs) {
			Py_DecRef(pArg);
		}
		Py_DecRef(pFunc);
		Py_DecRef(pArgList);
		if (!pRet) {
			std::cerr << "call func failed." << std::endl;
			return 0;
		}
		_RetTp ret;
		if constexpr (std::is_integral<_RetTp>::value) {
			PyArg_Parse(pRet, "i", &ret);
		}
		else if constexpr (std::is_floating_point<_RetTp>::value) {
			PyArg_Parse(pRet, "d", &ret);
		}
		else if constexpr (std::is_void<_RetTp>::value) {
			Py_DecRef(pRet);
			return;
		}
		Py_DecRef(pRet);
		return ret;
	}
private:
	template<typename _Tp, typename... _Args>
	void SetArgList(PyObject *pArgList, _Tp arg, _Args... args) {
		size_t index = PyTuple_Size(pArgList) - sizeof...(args) - 1;
		if constexpr (std::is_integral<_Tp>::value) {
			pArgs[index] = Py_BuildValue("i", arg);
		}
		else if constexpr (std::is_floating_point<_Tp>::value) {
			pArgs[index] = Py_BuildValue("d", arg);
		}
		PyTuple_SetItem(pArgList, index, pArgs[index]);
		SetArgList(pArgList, args...);
	}
	template<typename _Tp>
	void SetArgList(PyObject *pArgList, _Tp arg) {
		size_t index = PyTuple_Size(pArgList) - 1;
		if constexpr (std::is_integral<_Tp>::value) {
			pArgs[index] = Py_BuildValue("i", arg);
		}
		else if constexpr (std::is_floating_point<_Tp>::value) {
			pArgs[index] = Py_BuildValue("d", arg);
		}
		PyTuple_SetItem(pArgList, index, pArgs[index]);
	}
	void SetArgList(PyObject *pArgList) {
	}
private:
	PyObject *pModule;
	PyObject *pFunc;
	std::vector<PyObject *> pArgs;
};

class PyRunner {
public:
	PyRunner() : pModule(nullptr){
		Py_Initialize();
	}
	PyRunner(std::string module) {
		Py_Initialize();
		PyRun_SimpleString("import sys");
		std::string ModulePath("");
		std::regex pathReg("^.+/");
		std::smatch m;
		if (std::regex_match(module, m, std::regex(std::string(".*\\.py")))) {
			module.resize(module.size()-3);
		}
		if (std::regex_search(module, m, pathReg)) {
			PyRun_SimpleString(("sys.path.append('"+m.str()+"')").c_str());
			pModule = PyImport_ImportModule(
				module.substr(m.position()+m.str().size(), module.size()).c_str());
		}
		else {
			PyRun_SimpleString("sys.path.append('./')");
			pModule = PyImport_ImportModule(module.c_str());
		}
		if (!pModule) {
			std::cerr << "get module failed." << std::endl;
		}
	}
	~PyRunner() {
		if (pModule) {
			Py_DecRef(pModule);
		}
		Py_Finalize();
	}
	PyRunner(PyRunner &) = default;
	PyRunner(PyRunner &&_old) {
		pModule = _old.pModule;
		_old.pModule = nullptr;
	}
	PyRunner &operator=(PyRunner &) = default;
	PyRunner &operator=(PyRunner &&_old) {
		pModule = _old.pModule;
		_old.pModule = nullptr;
		return *this;
	}
public:
	void SimpleString(std::string str) {
		PyRun_SimpleString(str.c_str());
	}
	template<typename _RetTp>
	PyFunctor<_RetTp> CallFunc(std::string funcName) {
		if (pModule) {
			auto pFunc = PyObject_GetAttrString(pModule, funcName.c_str());
			if (!pFunc) {
				std::cerr << "get func failed." << std::endl;
			}
			return PyFunctor<_RetTp>(pModule, pFunc);
		}
		return PyFunctor<_RetTp>(nullptr, nullptr);
	}
private:
	PyObject *pModule;
};

#endif//_PYRUNNER_HPP_
