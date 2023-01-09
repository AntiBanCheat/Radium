#pragma once

#include "../../ModuleManager.h"
#include "../Module.h"

class Regen : public IModule {
private:
    bool selectPickaxe();
    float animYaw = 0.f;
    vec3_ti blockPos;
    vec3_t tempblockPos;

    float range = 4.5f;
    int delay = 6;
    float animYawB;
    float animPitchB;
    bool smooth = false;

    int tick = 0;
    int slot = 0;
    bool rot = false;
    bool bypassrot = false;
    bool firstbreak = false;
    bool continuemine = false;
    int rottime = 0;
    vec3_t OldBlockPos;
public:
    int enabledticks;
    bool hasDestroyed = false;
    bool destroy = false;
    int blocksBroke = 0;
    bool swing = false;
    bool rotation = false;
    bool isregen = false;
    bool canattack = true;

    std::string name = "Regen";
    SettingEnum mode = this;

    virtual void onSendPacket(C_Packet* packet);
    virtual void onPlayerTick(C_Player* plr);
    virtual const char* getRawModuleName();
    virtual const char* getModuleName();
    virtual void onTick(C_GameMode* gm);
    virtual void onDisable();
    virtual void onEnable();
    Regen();
};