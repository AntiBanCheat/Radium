#include "HiveFlytwo.h"

HiveFlytwo::HiveFlytwo() : IModule(0, Category::MOVEMENT, "How the fuck does this bypass ?!?!?") {
	registerFloatSetting("Speed", &speed, speed, .1f, 2.f);
	registerFloatSetting("Height", &height, height, 0.f, 1.f);
	registerFloatSetting("ClipUp", &clipUp, clipUp, 0.f, 5.f);
	registerFloatSetting("Timer", &timer, timer, 0.f, 30.f);
	registerIntSetting("DashTime", &dashTime, dashTime, 0, 3000);
	registerBoolSetting("Freelook", &lock, lock);
	registerBoolSetting("ClipFly", &tp, tp);
	registerBoolSetting("ClipLimit", &cliplimit, cliplimit);
	registerIntSetting("ClipTimes", &cliptimes, cliptimes, 1, 20);
	registerFloatSetting("ClipValue", &clipvalue, clipvalue, 0.f, 2.f);
}
vec3_t savePos;
int nowtimes;
const char* HiveFlytwo::getModuleName() {
	return ("HiveFly2");
}


void HiveFlytwo::onEnable() {
	dashed = false;
	nowtimes = 0;
	auto player = g_Data.getLocalPlayer();
	savePos = *player->getPos();
	vec3_t myPos = *player->getPos();
	if (!tp)
	{
		myPos.y += clipUp;
		player->setPos(myPos);
	}
	if (lock)
	{
		auto lock = moduleMgr->getModule<Freelook>();
		lock->setEnabled(true);
	}
	auto aura = moduleMgr->getModule<Killaura>();
	if (aura->isEnabled())
	{
		aurais = true;
		aura->setEnabled(false);
	}
	g_Data.getClientInstance()->minecraft->setTimerSpeed(timer);
}

void HiveFlytwo::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
}

void HiveFlytwo::onMove(C_MoveInputHandler* input) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	vec3_t moveVec;
	bool blink = false;
	if (g_Data.canUseMoveKeys()) {
		if (TimerUtil::hasTimedElapsed(dashTime, !blink) && !dashed) {
			dashed = true;
			if(tp)
			{
				if (cliplimit)
				{
					if (nowtimes > cliptimes)
					{
						auto two = moduleMgr->getModule<HiveFlytwo>();
						two->setEnabled(false);
					}
					else
					{
						{
							nowtimes++;
							MoveUtil::setSpeed(speed);
							vec3_t nowPos = *player->getPos();
							savePos.y -= clipvalue;
							nowPos.y = savePos.y - clipvalue;
							player->setPos(nowPos);
							player->velocity.y = 0;
						}
					}
				}
				else
				{
					MoveUtil::setSpeed(speed);
					vec3_t nowPos = *player->getPos();
					savePos.y -= clipvalue;
					nowPos.y = savePos.y;
					player->setPos(nowPos);
					player->velocity.y = 0;
				}
			}
			else
			{
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
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	MoveUtil::fullStop(false);
	if (lock)
	{
		auto lock = moduleMgr->getModule<Freelook>();
		lock->setEnabled(false);
	}
	auto aura = moduleMgr->getModule<Killaura>();
	if (aurais == true)
	{
		aura->setEnabled(true);
		aurais = false;
	}
}
