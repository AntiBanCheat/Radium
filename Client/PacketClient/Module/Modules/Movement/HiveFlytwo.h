#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class HiveFlytwo : public IModule {
private:
	bool dashed = false;
	bool aurais = false;
	float speed = 0.5;
	float height = 0.0;
	float clipUp = 1.0;
	int dashTime = 0;
	int boostTime = 200;
	int lockTime = 1500;
public:
	virtual void onMove(C_MoveInputHandler* input);
	virtual const char* getModuleName();
	virtual void onTick(C_GameMode* gm);
	virtual void onDisable();
	virtual void onEnable();
	HiveFlytwo();
};
