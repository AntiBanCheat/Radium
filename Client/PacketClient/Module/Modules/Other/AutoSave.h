#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class AutoSave : public IModule {
public:
    std::string name = "AutoSave";
    virtual void onTick(C_GameMode* gm);
    virtual const char* getModuleName();
    virtual void onEnable();
    AutoSave();
};
