#include "ScriptManager.h"
#include <object.h>
#include <pythonrun.h>

namespace Script{
template <> PyObject* GetPyObj<bool>(bool p)
{
    return p ? Py_True : Py_False;
}

template <> PyObject* GetPyObj<int>(int i)
{
    return PyInt_FromLong(i);
}

template <> PyObject* GetPyObj<unsigned int>(unsigned int i)
{
    return PyInt_FromLong(i);
}

template <> PyObject* GetPyObj<long>(long i)
{
    return PyInt_FromLong(i);
}

template <> PyObject* GetPyObj<float>(float i)
{
    return PyFloat_FromDouble(i);
}

template <> PyObject* GetPyObj<double>(double i)
{
    return PyFloat_FromDouble(i);
}


template <> PyObject* GetPyObj<char*>(char* s)
{
    return PyString_FromString(s);
}

template <> PyObject* GetPyObj<const char*>(const char* s)
{
    return PyString_FromString(s);
}

template <> PyObject* GetPyObj<std::string>(std::string s)
{
    return PyString_FromString(s.c_str());
}

}


ScriptManager::ScriptManager()
{
    Py_Initialize();
}

ScriptManager::~ScriptManager()
{
    Py_Finalize();
}

ScriptManager *ScriptManager::Instance()
{
    static ScriptManager* instance = ScriptManager::CreateScriptManager();
    //int argc = 1;
    //char** argv = (char**)CnMalloc(sizeof(char*)*argc);
    //instance->InitArgv(argc, argv);
    return instance;
}

void ScriptManager::CallPythonCommand(const std::string& pyCmd)
{
    PyRun_SimpleString(pyCmd.c_str());
}

void ScriptManager::Shell()
{
    CallPythonCommand("import code; code.interact(local=locals())");
}

PyObjectPtr ScriptManager::ImportPythonModule(const std::string& pyModuleName)
{
    PyObject* pModuleDict = nullptr;

    PyObjectMap::iterator citr = m_ModuleMap.find(pyModuleName);
    if (citr == m_ModuleMap.end())
    {
        PyObject *pModule = nullptr;
        PyObject *pDict = nullptr;

        pModule = PyImport_ImportModule(pyModuleName.c_str());
        if (NULL == pModule)
        {
            ProcessPythonException();
            return nullptr;
        }

        pDict = PyModule_GetDict(pModule);
        if (NULL == pDict)
        {
            ProcessPythonException();
            return nullptr;
        }

        m_ModuleMap[pyModuleName] = pModule;
        m_DictMap[pyModuleName] = pDict;

        pModuleDict = pModule;
    }
    else
    {
        pModuleDict = citr->second;
    }

    assert(pModuleDict);
    return pModuleDict;
}

PyObjectPtr ScriptManager::ReImportPythonModule(const std::string& pyModuleName)
{
    PyObjectMap::iterator citr = m_ModuleMap.find(pyModuleName);
    if (citr == m_ModuleMap.end())
    {
        return ImportPythonModule(pyModuleName);
    }
    else
    {
       PyObject* pModule = citr->second;

       PyObject* pDict = PyImport_ReloadModule(pModule);
       if(pDict == nullptr)
       {
           ProcessPythonException();           
           return nullptr;
       }
       else
       {
           m_DictMap[pyModuleName] = pDict;
           return pModule;
       }
    }
}

void ScriptManager::ReImportAllModule()
{
    for(PyObjectMap::iterator citr = m_ModuleMap.begin(); citr != m_ModuleMap.end(); ++citr)
    {
        PyObject* pModule = citr->second;
        std::string pyModuleName = citr->first;
//        DBG("reloading module: %s", pyModuleName);
        PyObject* pDict = PyImport_ReloadModule(pModule);
        if(pDict == nullptr)
        {
            ProcessPythonException();
            return;
        }
        else
        {
            m_DictMap[pyModuleName] = pDict;
        }
    }
}

PyObject* ScriptManager::HasPythonModule(const std::string& pyModuleName)
{
    PyObjectMap::iterator citr = m_ModuleMap.find(pyModuleName);
    if (citr == m_ModuleMap.end())
        return nullptr;
    else
        return citr->second;
}

bool ScriptManager::HasPythonFunc(const std::string& name)
{
    size_t p = name.rfind(".");
    std::string moduleName = name.substr(0, p);
    std::string funcName = name.substr(p + 1);

    // 获取模块的字典表
    PyObject* pDict = m_DictMap[moduleName];
    if(!pDict)
        return false;

    // 获取python模块的执行函数
    PyObject *pFunc = PyDict_GetItemString(pDict, funcName.c_str());
    if (!pFunc || !PyCallable_Check(pFunc))
        return false;

    return true;
}

void ScriptManager::InitArgv(int argc, char **argv)
{
    // 0 表示不更新python的sys.path.
    PySys_SetArgvEx(argc, argv, 0);
}

PyObject *ScriptManager::_GetPythonFunc(const std::string &name)
{
    size_t p = name.rfind(".");
    std::string moduleName = name.substr(0, p);
    std::string funcName = name.substr(p + 1);

    // 加载模块
    if(!HasPythonModule(moduleName))
    {
        if(!ImportPythonModule(moduleName))
            return nullptr;
    }

    // 获取模块的字典表
    PyObject* pDict = m_DictMap[moduleName];

    // 获取python模块的执行函数
    return PyDict_GetItemString(pDict, funcName.c_str());
}

PyObject *ScriptManager::_CallPythonObj(PyObject *callableObject, PyObject *pTupleArgs)
{
    std::cout<<"ScriptManager::_CallPythonObj "<<callableObject<<std::endl;
    if (!callableObject || !PyCallable_Check(callableObject))
    {
        ProcessPythonException();
        return nullptr;
    }

    PyObject *pResult = PyObject_CallObject(callableObject, pTupleArgs);
    if (!pResult)
    {

        ProcessPythonException();
    }
    return pResult;
}

PyObject *ScriptManager::_CallPythonFunc(const std::string& name, PyObject* pTupleArgs)
{
    PyObject *pFunc = _GetPythonFunc(name);

    return _CallPythonObj(pFunc, pTupleArgs);
}

void ScriptManager::ProcessPythonException()
{
    if(!PyErr_Occurred())
        return;

    PyObject *type_obj, *value_obj, *traceback_obj;
    PyErr_Fetch(&type_obj, &value_obj, &traceback_obj);
    if (value_obj == NULL)
        return;

    std::string strExceptionValue;

    PyErr_NormalizeException(&type_obj, &value_obj, 0);
    if (PyString_Check(PyObject_Str(value_obj)))
    {
        strExceptionValue = PyString_AsString(PyObject_Str(value_obj));
//        LOG_E << strExceptionValue.c_str() << std::endl;
        std::cout<<"ProcessPythonException "<<strExceptionValue<<std::endl;
    }

    std::string strError;

    if (traceback_obj != NULL)
    {
        PyObject* pModuleName = PyString_FromString("traceback");
        PyObject* traceModule = PyImport_Import(pModuleName);
        Py_XDECREF(pModuleName);
        if(traceModule != NULL)
        {
            PyObject* moduleDict = PyModule_GetDict(traceModule);
            if(moduleDict != NULL)
            {
                PyObject* func = PyDict_GetItemString(moduleDict,"format_exception");
                if(func != NULL)
                {
                    PyObject* errList = PyObject_CallFunctionObjArgs(func, type_obj, value_obj, traceback_obj, NULL);
                    if(errList != NULL)
                    {
                        int listSize = PyList_Size(errList);
                        for(int i=0; i<listSize; ++i)
                        {
                            strError = PyString_AsString(PyList_GetItem(errList, i));
//							LOG_E << strError.c_str() << std::endl;
                            std::cout<<"ProcessPythonException "<<strError<<std::endl;

                        }
                    }
                }
            }
            Py_XDECREF(traceModule);
        }
    }

    Py_XDECREF(type_obj);
    Py_XDECREF(value_obj);
    Py_XDECREF(traceback_obj);

    if (strExceptionValue == "abort")
        abort();
}
