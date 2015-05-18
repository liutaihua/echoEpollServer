#include "ScriptManager.h"
#include "GameLogicScript.h"
#include "eplnetwork/ServerClientSocket.h"
#include "eplnetwork/Socket.h"
#include "echo_wrap.cxx"


namespace Script
{

/*
下面是宏 DEFINE_GetPyObj_FOR 生成代码的样例

template <> PyObject* GetPyObj<WorldObject*>(WorldObject* p)
{
    const int disown = 0;
    return SWIG_NewPointerObj(p, SWIGTYPE_p_WorldObject, disown);
}

*/

#define DEFINE_GetPyObj_FOR(__type) \
    template <> PyObject* GetPyObj<__type*>(__type* p) \
    { \
        const int disown = 0; \
        return SWIG_NewPointerObj(p, SWIGTYPE_p_##__type, disown); \
    }

DEFINE_GetPyObj_FOR(SClientSocket)
DEFINE_GetPyObj_FOR(TcpSocket)


#undef DEFINE_GetPyObj_FOR

}

void init_script()
{
    SWIG_init();
}

int double_it(int a)
{
    return a * 2;
}


static std::string g_funInfos[] =
{
    "Broken GM command help"
};

void help()
{
    for (uint32 i = 0; i < sizeof(g_funInfos) / sizeof(g_funInfos[0]); i++)
    {
        PyStdOut((g_funInfos[i]).c_str());
    }
}

void PyStdOut(const char* p)
{
    std::cout<<p<<std::endl;
//    GMCommand::GetInstance().Output(p);
//    PY(p);
}

