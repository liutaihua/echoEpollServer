#include "GameLogicScriptManager.h"
#include "GameLogicScript.h"
#include "ScriptManager.h"

ScriptManager* ScriptManager::CreateScriptManager()
{
    return new GameLogicScriptManager;
}

GameLogicScriptManager::GameLogicScriptManager()
{
    init_script();
    CallPythonCommand("import sys, os");
    std::cout<<"GameLogicScriptManager constrator"<<std::endl;

//    // 添加脚本目录
    CallPythonCommand("sys.path.append(os.path.join(os.path.abspath('.'), 'Py'))");
//    // 导入模块
    CallPythonCommand("import ttt");
    CallPythonCommand("import eplObserver");

    // 重定向python的stdout&stderr
    Redirect();
}

GameLogicScriptManager::~GameLogicScriptManager()
{

}

void GameLogicScriptManager::Redirect()
{
    CallPythonCommand("import redirect; redirect.redirect_output()");
    std::cout<<"Redirect done"<<std::endl;
}

