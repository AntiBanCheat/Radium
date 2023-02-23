#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class AutoTool : public IModule {
public:
    std::string name = "AutoTool";
    bool hasClicked = false;

    // Inherited via IModule
    //virtual void onMove(C_MoveInputHandler* input) override;
    //virtual void onSendPacket(C_Packet* packet) override;
    // virtual void onPlayerTick(C_Player* plr) override;
    //virtual const char* getRawModuleName() override;
    virtual const char* getModuleName() override;
    virtual void onTick(C_GameMode* gm) override;
    virtual void onEnable() override;
    AutoTool();
};