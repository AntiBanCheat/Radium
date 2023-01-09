#include "HiveFlytwo.h"

HiveFlytwo::HiveFlytwo() : IModule(0, Category::MOVEMENT, "How the fuck does this bypass ?!?!?") {
	registerFloatSetting("Speed", &speed, speed, .1f, 2.f);
	registerFloatSetting("Height", &height, height, 0.f, 1.f);
	registerIntSetting("Blinktime", &blinkTime, blinkTime, 0, 1000);
	registerIntSetting("Dashtime", &dashTime, dashTime, 0, 1000);
	registerBoolSetting("Blink", &blink, blink);
}

const char* HiveFlytwo::getModuleName() {
	return ("HiveFly2");
}


void HiveFlytwo::onEnable() {
	dashed = false;
}

void HiveFlytwo::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	if (g_Data.canUseMoveKeys()) {
		if (blink) {
			auto blink = moduleMgr->getModule<Blink>();
			if (TimerUtil::hasTimedElapsed(blinkTime, true)) {
				blink->setEnabled(false);
				blink->setEnabled(true);
			}
		}

	}
}

void HiveFlytwo::onMove(C_MoveInputHandler* input) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	vec3_t moveVec;

	if (g_Data.canUseMoveKeys()) {
		if (TimerUtil::hasTimedElapsed(dashTime, !blink) && !dashed) {
			dashed = true;
			float calcYaw = (player->yaw + 90) * (PI / 180);
			vec2_t moveVec2d = { input->forwardMovement, -input->sideMovement };
			bool pressed = moveVec2d.magnitude() > 0.01f;
			float c = cos(calcYaw);
			float s = sin(calcYaw);
			moveVec2d = { moveVec2d.x * c - moveVec2d.y * s, moveVec2d.x * s + moveVec2d.y * c };

			if (player->onGround && pressed)
				player->jumpFromGround();
			moveVec.x = moveVec2d.x * speed;
			moveVec.y = height;
			player->velocity.y;
			moveVec.z = moveVec2d.y * speed;
			if (pressed) player->lerpMotion(moveVec);
		}
		else {
			dashed = false;
		}
	}
}

void HiveFlytwo::onDisable() {
	uintptr_t ViewBobbing = FindSignature("0F B6 80 DB 01 00 00");
	Utils::patchBytes((unsigned char*)ViewBobbing, (unsigned char*)"\x0F\xB6\x80\xDB\x01\x00\x00", 7);
	g_Data.getClientInstance()->minecraft->setTimerSpeed(20.f);
	auto speedMod = moduleMgr->getModule<Speed>();
	auto blink = moduleMgr->getModule<Blink>();
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	MoveUtil::fullStop(false);
	blink->setEnabled(false);
}