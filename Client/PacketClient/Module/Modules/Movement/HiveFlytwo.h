#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class HiveFlytwo : public IModule {
private:
	bool dashed = false;
	bool blink = false;
	float speed = 0.5;
	float height = 0.1;
	int blinkTime = 500;
	int dashTime = 450;
public:
	virtual void onMove(C_MoveInputHandler* input);
	virtual const char* getModuleName();
	virtual void onTick(C_GameMode* gm);
	virtual void onDisable();
	virtual void onEnable();
	HiveFlytwo();
};
