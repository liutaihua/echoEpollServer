#pragma once

#include "ScriptManager.h"


class GameLogicScriptManager : public ScriptManager
{
public:
	GameLogicScriptManager();
    ~GameLogicScriptManager();

    const PyObjectPtr& GetGameLogicModule() const
    {
        return m_pGameLogicModule;
    }

private:
     void Redirect();
    PyObjectPtr m_pGameLogicModule;
};

