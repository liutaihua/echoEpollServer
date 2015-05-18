#pragma once

#include "PyWrap.h"

#include <Python.h>
#include <boost/python/extract.hpp>
//#include <sdsingleton.h>
#include "eplnetwork/Common.h"
#include <map>
#include <object.h>
#include <pythonrun.h>



namespace Script
{
template <typename T> PyObject* GetPyObj(T obj);

}




class ScriptManager
{
public:
    static ScriptManager* CreateScriptManager();
    virtual ~ScriptManager();

    static ScriptManager* Instance();

    PyObjectPtr ImportPythonModule(const std::string& pyModuleName);
    PyObjectPtr ReImportPythonModule(const std::string& pyModuleName);
    void ReImportAllModule();

    void CallPythonCommand(const std::string& pyCmd);
    void Shell();

    bool HasPythonFunc(const std::string& name);

    void InitArgv(int argc, char** argv);

    template <typename P1>
    PyObject *MakeTuple(P1 arg1)
    {
        PyObject *pyTupleArgs = PyTuple_New(1);
        PyTuple_SetItem(pyTupleArgs, 0, Script::GetPyObj(arg1));

        return pyTupleArgs;
    }

    template <typename P1, typename P2>
    PyObject *MakeTuple(P1 arg1, P2 arg2)
    {
        PyObject *pyTupleArgs = PyTuple_New(2);
        PyTuple_SetItem(pyTupleArgs, 0, Script::GetPyObj(arg1));
        PyTuple_SetItem(pyTupleArgs, 1, Script::GetPyObj(arg2));

        return pyTupleArgs;
    }

    template <typename P1, typename P2, typename P3>
    PyObject *MakeTuple(P1 arg1, P2 arg2, P3 arg3)
    {
        PyObject *pyTupleArgs = PyTuple_New(3);
        PyTuple_SetItem(pyTupleArgs, 0, Script::GetPyObj(arg1));
        PyTuple_SetItem(pyTupleArgs, 1, Script::GetPyObj(arg2));
        PyTuple_SetItem(pyTupleArgs, 2, Script::GetPyObj(arg3));

        return pyTupleArgs;
    }

    template <typename P1, typename P2, typename P3, typename P4>
    PyObject *MakeTuple(P1 arg1, P2 arg2, P3 arg3, P4 arg4)
    {
        PyObject *pyTupleArgs = PyTuple_New(4);
        PyTuple_SetItem(pyTupleArgs, 0, Script::GetPyObj(arg1));
        PyTuple_SetItem(pyTupleArgs, 1, Script::GetPyObj(arg2));
        PyTuple_SetItem(pyTupleArgs, 2, Script::GetPyObj(arg3));
        PyTuple_SetItem(pyTupleArgs, 3, Script::GetPyObj(arg4));

        return pyTupleArgs;
    }

    template <typename P1>
    void CallNoRT(const std::string& name, P1);

    template <typename P1, typename P2>
    void CallNoRT(const std::string& name, P1, P2);

    template <typename P1, typename P2, typename P3>
    void CallNoRT(const std::string& name, P1, P2, P3);

    template <typename P1, typename P2, typename P3, typename P4>
    void CallNoRT(const std::string& name, P1 arg1, P2 arg2, P3 arg3, P4 arg4);

    template <typename P1>
    void CallNoRT(PyObject* callable, P1);

    template <typename P1, typename P2>
    void CallNoRT(PyObject* callable, P1, P2);

    template <typename P1, typename P2, typename P3>
    void CallNoRT(PyObject* callable, P1, P2, P3);

    template <typename P1, typename P2, typename P3, typename P4>
    void CallNoRT(PyObject* callable, P1 arg1, P2 arg2, P3 arg3, P4 arg4);

    template <typename R, typename P1>
    R Call1(const std::string& name, P1 arg1, R nullvalue);

    template <typename R, typename P1, typename P2>
    R Call2(const std::string& name, P1 arg1, P2 arg2, R nullvalue);

    template <typename R, typename P1, typename P2, typename P3>
    R Call3(const std::string& name, P1 arg1, P2 arg2, P3 arg3, R nullvalue);

    template <typename R, typename P1, typename P2, typename P3, typename P4>
    R Call4(const std::string& name, P1 arg1, P2 arg2, P3 arg3, P4 arg4, R nullvalue);

    template <typename R, typename P1>
    R Call1(PyObject* callable, P1 arg1, R nullvalue);

    template <typename R, typename P1, typename P2>
    R Call2(PyObject* callable, P1 arg1, P2 arg2, R nullvalue);

    template <typename R, typename P1, typename P2, typename P3>
    R Call3(PyObject* callable, P1 arg1, P2 arg2, P3 arg3, R nullvalue);

    template <typename R, typename P1, typename P2, typename P3, typename P4>
    R Call4(PyObject* callable, P1 arg1, P2 arg2, P3 arg3, P4 arg4, R nullvalue);

    template <typename R>
    R Call0(PyObject* callable, R nullvalue);

    int32 GetModuleNumber() {return m_ModuleMap.size();}
    
    typedef std::map<std::string, PyObject*>    PyObjectMap;
    const PyObjectMap& GetModuleMap() { return m_ModuleMap; }
protected:
	ScriptManager();
	
    PyObject* HasPythonModule(const std::string& pyModuleName);
    PyObject* _GetPythonFunc(const std::string& name);
    PyObject* _CallPythonObj(PyObject* callableObject, PyObject* pTupleArgs);
    PyObject* _CallPythonFunc(const std::string& name, PyObject* pTupleArgs);

    void ProcessPythonException();

private:
    
    PyObjectMap     m_ModuleMap;        // 模块表
    PyObjectMap     m_DictMap;          // 模块字典表
};

template <typename P1>
void ScriptManager::CallNoRT(const std::string& name, P1 arg1)
{

    PyObject *pyTupleArgs = MakeTuple(arg1);
    auto r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1, typename P2>
void ScriptManager::CallNoRT(const std::string& name, P1 arg1, P2 arg2)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2);

    auto r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1, typename P2, typename P3>
void ScriptManager::CallNoRT(const std::string& name, P1 arg1, P2 arg2, P3 arg3)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3);

    auto r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1, typename P2, typename P3, typename P4>
void ScriptManager::CallNoRT(const std::string& name, P1 arg1, P2 arg2, P3 arg3, P4 arg4)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3, arg4);

    auto r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1>
void ScriptManager::CallNoRT(PyObject* callable, P1 arg1)
{
    PyObject *pyTupleArgs = MakeTuple(arg1);

    auto r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1, typename P2>
void ScriptManager::CallNoRT(PyObject* callable, P1 arg1, P2 arg2)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2);

    auto r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1, typename P2, typename P3>
void ScriptManager::CallNoRT(PyObject* callable, P1 arg1, P2 arg2, P3 arg3)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3);

    auto r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename P1, typename P2, typename P3, typename P4>
void ScriptManager::CallNoRT(PyObject* callable, P1 arg1, P2 arg2, P3 arg3, P4 arg4)
{
//    DBG("ofewjfoew CallNoRT4");
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3, arg4);

    auto r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    Py_XDECREF(r);
}

template <typename R, typename P1>
R ScriptManager::Call1(const std::string& name, P1 arg1, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1);

    PyObject* r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1, typename P2>
R ScriptManager::Call2(const std::string& name, P1 arg1, P2 arg2, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2);

    PyObject* r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    // r 需要减少引用计数吗?
    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1, typename P2, typename P3>
R ScriptManager::Call3(const std::string& name, P1 arg1, P2 arg2, P3 arg3, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3);

    PyObject* r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    // r 需要减少引用计数吗?
    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1, typename P2, typename P3, typename P4>
R ScriptManager::Call4(const std::string& name, P1 arg1, P2 arg2, P3 arg3, P4 arg4, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3, arg4);

    PyObject* r = _CallPythonFunc(name, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    // r 需要减少引用计数吗?
    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1>
R ScriptManager::Call1(PyObject* callable, P1 arg1, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1);

    PyObject* r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1, typename P2>
R ScriptManager::Call2(PyObject* callable, P1 arg1, P2 arg2, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2);

    PyObject* r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1, typename P2, typename P3>
R ScriptManager::Call3(PyObject* callable, P1 arg1, P2 arg2, P3 arg3, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3);

    PyObject* r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R, typename P1, typename P2, typename P3, typename P4>
R ScriptManager::Call4(PyObject* callable, P1 arg1, P2 arg2, P3 arg3, P4 arg4, R nullvalue)
{
    PyObject *pyTupleArgs = MakeTuple(arg1, arg2, arg3, arg4);

    PyObject* r = _CallPythonObj(callable, pyTupleArgs);
    Py_XDECREF(pyTupleArgs);
    if (!r)
        return nullvalue;

    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

template <typename R>
R ScriptManager::Call0(PyObject* callable, R nullvalue)
{
    PyObject* r = _CallPythonObj(callable, nullptr);
    if (!r)
        return nullvalue;

    R ret = boost::python::extract<R>(r);
    Py_DECREF(r);
    return ret;
}

