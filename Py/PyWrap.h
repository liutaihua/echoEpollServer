#pragma once

#include <vector>
struct _object;
typedef _object PyObject;


class PyObjectPtr
{
public:
    PyObjectPtr(PyObject* pObj = nullptr, bool incref = false);
       
    PyObjectPtr(const PyObjectPtr& obj);
        
    ~PyObjectPtr();
    
    PyObject* Get() const  { return m_pObj; }
    PyObject* operator ->() { return Get(); }

    typedef PyObject* PyObjectPtr::*unspecified_bool_type;

    operator unspecified_bool_type() const // never throws
    {
        return m_pObj == nullptr ? 0 : &PyObjectPtr::m_pObj;
    }
    PyObjectPtr& operator=(const PyObjectPtr& rhs);

private:
    PyObject* m_pObj;
};

class PyArgsBuilder
{
public:
    void Add(int arg);
    void Add(PyObjectPtr&& pObject);
    PyObjectPtr GetArgs() const;

private:
    std::vector<PyObjectPtr> m_args;
};
