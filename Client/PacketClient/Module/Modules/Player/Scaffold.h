#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class Scaffold : public IModule {
private:
	bool shouldSwing = false;
	bool foundBlock = false;
	bool blockCount = true;
	bool showExposed = true;
	bool movedx = false;
	bool movedz = false;
	bool speedwasenabled = false;
	vec3_t oldpos;
	int expR = 255;
	int expG = 50;
	int expB = 0;
	float expT = 0.4f;
	bool inclSelf = true;
	float animPitch = 0.f;
	float animBack = 0.f;
	float animFlareon = 0.f;
	float animFlareonPitch = 0.f;
	float animYaw = 0.f;
	int towerTimer = 20;
	float random = 1.5f;
	bool sprint = true;
	bool lockY = false;
	bool spoof = false;
	bool zipline = false;
	bool preventkicks = false;
	int yLock = -1;
	int timer = 20;
	int range = 4;
	int slot = 0;
	vec3_t blockBelowY;
	bool fake = false;

	bool sneaking = false;
	bool jumping = false;
	bool oneDot12 = true;

	bool findBlocks(C_ItemStack* itemStack);
	bool isUsefulBlock(C_ItemStack* itemStack);
	bool buildBlock(vec3_t blockBelow);
	bool predictBlock(vec3_t blockBelow);
	bool isBlockReplacable(vec3_t blockPos);
	bool selectBlock();

	// Positions
	vec3_t calcPos = vec3_t{0, 0, 0};
	vec3_t initialPos;
	vec3_t blockPos;
	vec3_t backPos;
	vec3_t flareonpos;
	vec3_t flareonpos2;

public:
	float currExtend = 0.f;
	bool swing = true;
	float Range = 4;
	float scY = 20.f;
	float scX = 10.f;
	float extend = 1;

	std::vector<vec3_ti> highlightCity;
	std::vector<vec3_t> exposee;
	std::string name = ("Scaffold");
	SettingEnum rotations = this;
	SettingEnum downwards = this;
	SettingEnum tower = this;
	SettingEnum type = this;
	bool isBlockAGoodCity(vec3_ti* blk, vec3_ti* personPos);

	virtual void onPreRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onMove(C_MoveInputHandler* input) override;
	virtual void onSendPacket(C_Packet* packet) override;
	virtual void onPlayerTick(C_Player* plr) override;
	virtual const char* getRawModuleName() override;
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	//virtual void onWorldTick(C_GameMode* gm) override;
	virtual void onDisable() override;
	virtual void onEnable() override;
	Scaffold();
};