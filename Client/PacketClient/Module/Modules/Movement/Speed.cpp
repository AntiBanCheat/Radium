#include "Speed.h"
#include "../pch.h"

bool testOnGround = false;
float speedMax = 0.69f; // inf value
float speedMin = 0.59f; // inf value
int packetsSent = 0;
int enabledTicks = 0;
int flareonticks = 0;
<<<<<<< HEAD
int strafeTime = 0;
=======
int dmgticks = 0;
>>>>>>> parent of 7bd3f58 (DamageSafe renamed to DamageBoost and improving bypass)
int hivegroundticks = 0;

using namespace std;
Speed::Speed() : IModule(0, Category::MOVEMENT, "Increases your speed") {
	registerEnumSetting("Mode", &mode, 0);
	mode.addEntry("Vanilla", 0);
	mode.addEntry("Hive", 1);
	mode.addEntry("HiveSafe", 2);
	mode.addEntry("HiveGround", 3);
	mode.addEntry("Cubecraft", 4);
	mode.addEntry("Friction", 5);
	mode.addEntry("Boost", 6);
	mode.addEntry("Test", 7);
	mode.addEntry("NoGround", 8);
	//mode.addEntry("Packet", 9);
	mode.addEntry("TPBoost", 9);
	mode.addEntry("Halcyon", 10);
	mode.addEntry("Flareon", 11);
	mode.addEntry("DamageSafe", 12);
	// Vanilla
	registerFloatSetting("Height", &height, height, 0.000001f, 0.40f);
	// All Settings
	registerFloatSetting("Speed", &speed, speed, 0.2f, 2.f);
	registerIntSetting("Timer", &timer, timer, 20, 35);
	registerBoolSetting("NoSlabs", &noslabs, noslabs);
	registerBoolSetting("DesnycBoost", &dboost, dboost);

	// Friction
	registerFloatSetting("SpeedMax", &speedMax, speedMax, 0.f, 2.f);
	registerFloatSetting("SpeedMin", &speedMin, speedMin, 0.f, 2.f);
	registerFloatSetting("Duration", &duration, duration, 0.5f, 1.05f);
}

const char* Speed::getRawModuleName() {
	return "Speed";
}

const char* Speed::getModuleName() {
	name = string("Speed ") + string(GRAY) + mode.GetEntry(mode.getSelectedValue()).GetName();
	return name.c_str();
}

void Speed::onEnable() {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	enabledTicks = 0;
	flareonticks = 0;
<<<<<<< HEAD
	strafeTime = 0;
=======
	dmgticks = 0;
>>>>>>> parent of 7bd3f58 (DamageSafe renamed to DamageBoost and improving bypass)
	clientmessage = false;
	hivegroundticks = 0;
	needblink = false;

	oldx = player->currentPos.x;
	oldz = player->currentPos.z;
}

void Speed::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	if (mode.getSelectedValue() == 11) player->setSprinting(true); 

	if (enabledTicks > 10) enabledTicks++;
	else enabledTicks = 0;

	if (player->onGround) { groundTicks++; offGroundTicks = 0; }
	else { offGroundTicks++; groundTicks = 0; }

	if (noslabs) player->stepHeight = 0.3f;

	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	g_Data.getClientInstance()->minecraft->setTimerSpeed(timer);

	// HiveLow
	switch (mode.getSelectedValue()) {
	case 3: // Cubecraft
		ticks++;
		break;
	}
if (dboost) {
		if (player->onGround) {
			moduleMgr->getModule<Blink>()->setEnabled(false);
			//auto notification = g_Data.addNotification("Speed:", "IKA HA BAKA"); notification->duration = 3;
		}
	}
	if (dboost) {

		if (GameData::isKeyDown(*input->spaceBarKey))g_Data.getClientInstance()->minecraft->setTimerSpeed(boosttimer);
		if (GameData::isKeyDown(*input->spaceBarKey)) {
			if (player->onGround) {
				moduleMgr->getModule<Blink>()->setEnabled(false);
				//auto notification = g_Data.addNotification("Speed:", "INSANE BYPASS"); notification->duration = 3;
			}
			else
			{
				moduleMgr->getModule<Blink>()->setEnabled(true);
			}
		}
	}
	if (dboost) {
		if (!GameData::isKeyDown(*input->spaceBarKey)) {
			moduleMgr->getModule<Blink>()->setEnabled(false);
		}
	}
}

void Speed::onMove(C_MoveInputHandler* input) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	bool pressed = MoveUtil::keyPressed();
	if (!pressed) MoveUtil::stop(false);
	if(mode.getSelectedValue() != 11) player->setSprinting(true);

	// Vanilla
	if (mode.getSelectedValue() == 0) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) { if (player->onGround && pressed) player->jumpFromGround(); useVelocity = false; } else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->velocity.y += height; useVelocity = false; }

		if (player->onGround && useVelocity && pressed && !input->isJumping) player->velocity.y = height;
		MoveUtil::setSpeed(speed);
	}

	// Hive
	if (mode.getSelectedValue() == 1) {
		speedFriction *= 0.9400610828399658f;
		if (pressed) {
			if (player->onGround) {
				player->jumpFromGround();
				speedFriction = randomFloat(0.5285087823867798f, 0.49729517102241516f);
			}
			else MoveUtil::setSpeed(speedFriction);
		}
	}

	// HiveSafe
	if (mode.getSelectedValue() == 2) {
		speedFriction *= 0.9400610828399658f;
		if (pressed) {
			if (player->onGround) {
				player->jumpFromGround();
				speedFriction = randomFloat(0.5085087823867798f, 0.47729517102241516f);
			}
			else MoveUtil::setSpeed(speedFriction);
		}
	}

	// HiveGround
	if (mode.getSelectedValue() == 3) {
		// eat my absctrionalie
		//if (player->onGround && pressed) { player->jumpFromGround(); }

		//speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				speedFriction = randomFloat(speedMin, speedMax);
				if (hivegroundticks < 1) {
					player->jumpFromGround();
					if (0 < player->velocity.y) player->velocity.y = 0.f;
					hivegroundticks = 3;
				}
				hivegroundticks--;
			}
		}
	}

	// Cubecraft
	if (mode.getSelectedValue() == 4) {
		float calcYaw = (player->yaw + 90) * (PI / 180);
		float c = cos(calcYaw);
		float s = sin(calcYaw);

		vec2_t moveVec2D = { input->forwardMovement, -input->sideMovement };
		moveVec2D = { moveVec2D.x * c - moveVec2D.y * s, moveVec2D.x * s + moveVec2D.y * c };
		vec3_t moveVec;

		speedFriction *= 0.97;
		if (ticks % 3 == 0) {
			moveVec.x = moveVec2D.x * speedFriction;
			moveVec.z = moveVec2D.y * speedFriction;
			moveVec.y = player->velocity.y;
		}
		else {
			g_Data.getClientInstance()->minecraft->setTimerSpeed(17.f);
			moveVec.x = moveVec2D.x * speedFriction / 3;
			moveVec.z = moveVec2D.y * speedFriction / 3;
			moveVec.y = player->velocity.y;
		}

		if (pressed) {
			player->lerpMotion(moveVec);
			if (player->onGround) {
				speedFriction = randomFloat(speedMin, speedMax);
				player->jumpFromGround();
			}
		}
	}

	// Friction
	if (mode.getSelectedValue() == 5) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) {
			if (player->onGround && pressed) { player->jumpFromGround(); useVelocity = false; }
		}
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->velocity.y += height; useVelocity = false; }

		speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				if (useVelocity && !input->isJumping) player->velocity.y = height;
				speedFriction = randomFloat(speedMin, speedMax);
			}
			else MoveUtil::setSpeed(speedFriction);
		}
	}

	// Fast / timerboost
	if (mode.getSelectedValue() == 9) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) {
			if (player->onGround && pressed) { player->jumpFromGround(); useVelocity = false; }
		}
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->velocity.y += height; useVelocity = false; }

		speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				if (useVelocity && !input->isJumping) player->velocity.y = height;
				speedFriction = randomFloat(speedMin, speedMax);
				g_Data.getClientInstance()->minecraft->setTimerSpeed(100.f);
			}
			else {
				MoveUtil::setSpeed(speedFriction);
				g_Data.getClientInstance()->minecraft->setTimerSpeed(timer);
			}
		}
	}

	// Halcyon
	if (mode.getSelectedValue() == 10) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) {
			if (player->onGround && pressed) { player->jumpFromGround(); useVelocity = false; }
		}
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->jumpFromGround(); player->velocity.y += height; useVelocity = false; }

		if(speedFriction > 0.25) speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				if (useVelocity && !input->isJumping) player->velocity.y = height;
				speedFriction = randomFloat(speedMin, speedMax);
			}
			else MoveUtil::setSpeed(speedFriction);
		}
	}

	// Flareon2
	if (mode.getSelectedValue() == 11) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) {
			if (player->onGround && pressed) { player->jumpFromGround(); useVelocity = false; }
		}
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->jumpFromGround(); player->velocity.y += height; useVelocity = false; }

		if (speedFriction > 0.25) speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				if (useVelocity && !input->isJumping) player->velocity.y = height;
				speedFriction = randomFloat(speedMin, speedMax);
			}
			else MoveUtil::setSpeed(speedFriction);
		}
	}

	/* HiveACDisabler
	if (clientmessage) {
		clientMessageF("AntiCheat got disabled");
		clientmessage = false;
	}

	if (mode.getSelectedValue() == 11) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) {
			if (player->onGround && pressed) { player->jumpFromGround(); useVelocity = false; }
		}
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->velocity.y += height; useVelocity = false; }

		speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				if (useVelocity && !input->isJumping) player->velocity.y = height;
				speedFriction = randomFloat(speedMin, speedMax);
				g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);
				g_Data.getClientInstance()->minecraft->setTimerSpeed(100.f);
			}
			else {
				MoveUtil::setSpeed(speedFriction);
				g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);
			}
		}
	}*/

	// Flareon
	if (mode.getSelectedValue() == 8) {
		static bool useVelocity = false;
		flareonticks++;
		// eat my absctrionalie
		if (height >= 0.385) { if (player->onGround && pressed) { player->jumpFromGround(); useVelocity = false; } }
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->velocity.y += height; useVelocity = false; }

		if (flareonticks >= 10 && speedFriction >= 0.4) {
			speedFriction = 0.2;
		}else speedFriction *= duration;
		if (pressed) {
			if (player->onGround) {
				if (useVelocity && !input->isJumping) player->velocity.y = height;
				speedFriction = randomFloat(speedMin, speedMax);
				MoveUtil::stop(false);
				flareonticks = 0;
			}
			else MoveUtil::setSpeed(speedFriction);
		}
	}

	// Boost
	if (mode.getSelectedValue() == 6) {
		static bool useVelocity = false;
		// eat my absctrionalie
		if (height >= 0.385) { if (player->onGround && pressed) player->jumpFromGround(); useVelocity = false; }
		else useVelocity = true;
		if (height <= 0.04 && !input->isJumping) { player->velocity.y += height; useVelocity = false; }

		if (pressed && player->onGround) {
			if (useVelocity && !input->isJumping) player->velocity.y = height;
			MoveUtil::setSpeed(speed);
		}
	}

	// Test
	if (mode.getSelectedValue() == 7) {
		float calcYaw = (player->yaw - 90) * (PI / 180);
		float calcPitch = (player->pitch) * (PI / 180);
		vec3_t rotVec;
		rotVec.x = cos(calcYaw) * randomFloat(.5f, 1.f);
		rotVec.y = sin(calcPitch) * randomFloat(.5f, 1.f);
		rotVec.z = sin(calcYaw) * randomFloat(.5f, 1.f);

		if (pressed) {
			if (MoveUtil::isMoving()) {
				C_MovePlayerPacket p2 = C_MovePlayerPacket(g_Data.getLocalPlayer(), player->getPos()->add(vec3_t(rotVec.x / randomFloat(1.1f, 2.2f), 0.f, rotVec.z / randomFloat(1.1f, 2.2f))));
				g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&p2);
				packetsSent++;
			}
			if (player->onGround) {
				speedFriction = randomFloat(speedMin, speedMax);
			} else {
				speedFriction = randomFloat(.33f, .45f);
			}
			MoveUtil::setSpeed(speedFriction);
		}
	}

	if (mode.getSelectedValue() == 12) {
		if (pressed) {
			dmgticks++;
			if (player->onGround) {
				player->jumpFromGround();
			}

			if (damageMotion != nullptr) {
				strafeTime = 0;
				MoveUtil::setSpeed(damageMotion.magnitudexz() + .06);
				damageMotion = nullptr;
			}

			if (MoveUtil::isMoving()) {
<<<<<<< HEAD
				if (strafeTime < 13) {
					strafeTime++;
=======
				if (player->damageTime > 0) {
					MoveUtil::setSpeed(speed);
					dmgticks = 0;
				}
				else if (dmgticks < 14) {
>>>>>>> parent of 7bd3f58 (DamageSafe renamed to DamageBoost and improving bypass)
					MoveUtil::setSpeed(player->velocity.magnitudexz());
				}
			}
		}
	}
}

void Speed::onSendPacket(C_Packet* packet) {
	C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();
	auto scaffold = moduleMgr->getModule<Scaffold>();
	auto player = g_Data.getLocalPlayer();
	auto* LatencyPacket = reinterpret_cast<NetworkLatencyPacket*>(packet);
	auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
	auto* authinputPacket = reinterpret_cast<PlayerAuthInputPacket*>(packet);
	NetworkLatencyPacket* netStack = (NetworkLatencyPacket*)packet;
	if (player == nullptr || input == nullptr) return;

	if (packet->isInstanceOf<C_MovePlayerPacket>()) {
		//packet
	}
}

void Speed::onDisable() {
	g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	MoveUtil::stop(false);

	preventKick = false;
	packetsSent = 0;
	needblink = false;
	player->stepHeight = 0.5625f;
}
