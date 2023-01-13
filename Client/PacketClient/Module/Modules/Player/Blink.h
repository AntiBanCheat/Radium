#pragma once
#include "../../ModuleManager.h"
#include "../Module.h"

class Blink : public IModule {
private:
	std::vector<PlayerAuthInputPacket*> PlayerAuthInputPacketHolder;
	std::vector<C_MovePlayerPacket*> MovePlayerPacketHolder;
	std::vector<C_PlayerActionPacket*> PlayerActionPacketHolder;
	std::vector<C_Packet*> PacketHolder;
	
public:
	inline std::vector<PlayerAuthInputPacket*>* getPlayerAuthInputPacketHolder() { return &PlayerAuthInputPacketHolder; };
	inline std::vector<C_MovePlayerPacket*>* getMovePlayerPacketHolder() { return &MovePlayerPacketHolder; };
	inline std::vector<C_PlayerActionPacket*>* getPlayerActionPacketHolder() { return &PlayerActionPacketHolder; };
	inline std::vector<C_Packet*>* getPacketHolder() { return &PacketHolder; };
	bool allpackets = false;
	virtual void onTick(C_GameMode* gm);
	virtual const char* getModuleName();
	Blink();
	~Blink();
};
