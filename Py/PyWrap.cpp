#include <iostream>
#include "PyWrap.h"

#include <Python.h>

PyObjectPtr::PyObjectPtr(PyObject* pObj, bool incref) :
    m_pObj(pObj)
{
    if (incref) Py_XINCREF(m_pObj);
}


PyObjectPtr::PyObjectPtr(const PyObjectPtr &obj) :
    m_pObj(obj.m_pObj)
{

    Py_XINCREF(m_pObj);
}

PyObjectPtr::~PyObjectPtr()
{
    Py_XDECREF(m_pObj);
}

PyObjectPtr& PyObjectPtr::operator=(const PyObjectPtr& rhs)
{
    PyObject* oldObj = m_pObj;
    m_pObj = rhs.m_pObj;
    Py_XINCREF(m_pObj);
    Py_XDECREF(oldObj);
    return *this;
}

void PyArgsBuilder::Add(int arg)
{
    PyObjectPtr pValue(PyInt_FromLong(arg));
    assert(pValue);
    m_args.push_back(pValue);
}

void PyArgsBuilder::Add(PyObjectPtr&& pObject)
{
    assert(pObject);
    m_args.push_back(pObject);
}

PyObjectPtr PyArgsBuilder::GetArgs() const
{
    PyObject* args = PyTuple_New(m_args.size());
    int i = 0;
    for (auto it = m_args.cbegin(); it != m_args.cend(); ++it, ++i)
    {
        PyTuple_SetItem(args, i, it->Get());
    }
    return PyObjectPtr(args);
}
