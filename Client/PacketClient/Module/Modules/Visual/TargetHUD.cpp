#include "TargetHUD.h"
#include "../pch.h"

using namespace std;
TargetHUD::TargetHUD() : IModule(0, Category::VISUAL, "Displays information about your target") {
	registerEnumSetting("Mode", &mode, 0);
	mode.addEntry("New", 0);
	mode.addEntry("Old", 1);
	mode.addEntry("Simple", 2);
	mode.addEntry("Novoline", 3);
	mode.addEntry("Pulsive", 4);
	mode.addEntry("PulsiveRGB", 5);
	mode.addEntry("PulsiveRGBOutline", 6);
	mode.addEntry("Pulsive4.0", 7);
	mode.addEntry("Rise", 8);
	registerBoolSetting("Animation", &animation, animation);
	registerBoolSetting("Items", &showItems, showItems);
	registerIntSetting("Opacity", &opacity, opacity, 0, 255);
	registerIntSetting("RiseQuality", &RiseQuality, RiseQuality, 1, 20);
}

const char* TargetHUD::getModuleName() { return ("TargetHUD"); }

#pragma region TargetList
static bool entityChanged = false;
static vector<C_Entity*> targetList;
void findPlayers_TargetHUD(C_Entity* currentEntity, bool isRegularEntity) {
	if (currentEntity == nullptr) return;
	if (currentEntity == g_Data.getLocalPlayer()) return;
	if (!g_Data.getLocalPlayer()->canAttack(currentEntity, false)) return;
	if (!g_Data.getLocalPlayer()->isAlive()) return;
	if (!currentEntity->isAlive()) return;
	if (currentEntity->getEntityTypeId() != 319) return;
	if (currentEntity->getNameTag()->getTextLength() <= 1) return;
	if (!TargetUtil::isValidTarget(currentEntity)) return;
	float dist = (*currentEntity->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
	if (dist < 8) targetList.push_back(currentEntity);
}
#pragma endregion

#pragma region TargetArray
struct CompareTargetEnArray {
	bool operator()(C_Entity* lhs, C_Entity* rhs) {
		C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
		return (*lhs->getPos()).dist(*localPlayer->getPos()) < (*rhs->getPos()).dist(*localPlayer->getPos());
	}
};
#pragma endregion

void TargetHUD::onTick(C_GameMode* gm) {
	targetList.clear();
	g_Data.forEachEntity(findPlayers_TargetHUD);
	targetListEmpty = targetList.empty();
	sort(targetList.begin(), targetList.end(), CompareTargetEnArray());

	if (!targetList.empty()) {
		entityChanged = true;
	}
	else {
		entityChanged = false;
		displayhealth = 0;
		size = 30.f;
		animationsize = 0.f;
	}
}

void TargetHUD::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	static auto clickGUI = moduleMgr->getModule<ClickGUIMod>();
	vec2_t windowSize = g_Data.getClientInstance()->getGuiData()->windowSize;

	if (g_Data.canUseMoveKeys() && !clickGUI->hasOpenedGUI) {

		if (entityChanged && animation) {
			if (mode.getSelectedValue() == 8) {
				positionX = windowSize.x / 2 + 10; positionY = windowSize.y / 2 + 10;
				if (animationsize > 1.f) animationsize -= ((animationsize - 1.f) / 5);
				else if (animationsize < 1.f) animationsize += ((1.f - animationsize) / 5);
				//animationsize = 1.f;
			}
			else {
				positionX += ((windowSize.x / 2 + 10) - positionX) * 0.045f; positionY += ((windowSize.y / 2 + 10) - positionY) * 0.045f;
			}
		}
		else { positionX = windowSize.x; positionY = windowSize.y; }

		if (!targetListEmpty) {
			auto absorbtion = 0.f;
			string percent = "100%";
			auto interfaceColor = ColorUtil::interfaceColor(1);
			vec3_t* pos = targetList[0]->getPos();
			auto health = targetList[0]->getHealth();
			int inthealth = health;
			int percentage = (health / 20) * 100;
			string displaypercent = to_string(percentage);
			if (displayhealth > health) displayhealth -= ((displayhealth - health) / 15);
			else if (displayhealth < health) displayhealth += ((health - displayhealth) / 15);
			string smoothtext = to_string(inthealth) + ".0 health left";
			float steveOpacity = (targetList[0]->damageTime * 80) / 8;
			float dist = (*targetList[0]->getPos()).dist(*g_Data.getLocalPlayer()->getPos());
			float defaultRectHeight = (5, 5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
			float novolineRectHeight = (3.25f, 3.25f) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
			if (showItems && mode.getSelectedValue() == 1) defaultRectHeight = (5, 2) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
			string position = "Position: " + to_string((int)floorf(pos->x)) + " " + to_string((int)floorf(pos->y)) + " " + to_string((int)floorf(pos->z));
			string distance = string(GRAY) + "Distance: " + string(RESET) + to_string((int)dist) + string(".") + to_string((int)(dist * 10) - ((int)dist * 10));

			string targetName = targetList[0]->getNameTag()->getText();
			targetName = Utils::sanitize(targetName);
			targetName = targetName.substr(0, targetName.find('\n'));

			if (!targetListEmpty) {
				if (mode.getSelectedValue() == 0) {
					string distance = string(GRAY) + "         Distance: " + string(RESET) + to_string((int)dist) + string(".") + to_string((int)(dist * 10) - ((int)dist * 10));
					string name = string(GRAY) + "         Name: " + string(RESET);
					string rawName = targetList[0]->getNameTag()->getText();
					rawName = Utils::sanitize(rawName);
					rawName = name + rawName;
					rawName = rawName.substr(0, rawName.find('\n'));
					if (rawName.length() < distance.length()) targetLen = DrawUtils::getTextWidth(&distance, 1) + 10.5;
					else targetLen = DrawUtils::getTextWidth(&rawName, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11);
					if (showItems) testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight);
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), false);
					vec4_t healthRect = vec4_t(testRect.x + 39, testRect.y + 32, testRect.x + ((targetLen - 4) / 20) * health, testRect.y + 34);
					if (showItems) healthRect = vec4_t(testRect.x + 4, testRect.y + 44, testRect.x + ((targetLen - 4) / 20) * health, testRect.y + 47);
					if (targetList[0]->damageTime > 1) {
						DrawUtils::drawRectangle(healthRect, MC_Color(255, 0, 0), 0.3f);
						DrawUtils::fillRectangleA(healthRect, MC_Color(255, 0, 0, steveOpacity));
					}
					else {
						DrawUtils::drawRectangle(healthRect, MC_Color(0, 255, 0), 0.3);
						DrawUtils::fillRectangleA(healthRect, MC_Color(0, 255, 0, 90));
					}
					vec4_t absorbtionRect = vec4_t(testRect.x + 39, testRect.y + 26, testRect.x + absorbtion * 4.f, testRect.y + 29);
					if (showItems) absorbtionRect = vec4_t(testRect.x + 4, testRect.y + 38, testRect.x + absorbtion * 4.f, testRect.y + 41);
					vec4_t absorbtionSubRect = vec4_t(testRect.x + 39, testRect.y + 28, testRect.x + targetLen - 4.f, testRect.y + 30);
					if (showItems) absorbtionSubRect = vec4_t(testRect.x + 4, testRect.y + 38, testRect.x + targetLen - 4.f, testRect.y + 41);
					DrawUtils::fillRectangleA(absorbtionSubRect, MC_Color(25, 25, 25, 255));

					if (absorbtion > 1.f) {
						DrawUtils::drawRectangle(absorbtionRect, MC_Color(255, 230, 0), 0.3);
						DrawUtils::fillRectangleA(absorbtionRect, MC_Color(255, 230, 0, 255));
					}

					DrawUtils::drawImage("textures/entity/steve.png", vec2_t(positionX + 5, positionY + 5), vec2_t(30, 30), vec2_t(0.125f, 0.125f), vec2_t(0.125f, 0.125f));
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(vec4_t(positionX + 5, positionY + 5, 35 + positionX, positionY + 35), MC_Color(255, 0, 0, steveOpacity));
					}
				}

				if (mode.getSelectedValue() == 4) { // pulsive
					float defaultRectHeight = (5.5, 5.5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					string name = "          ";
					string rawName = targetList[0]->getNameTag()->getText();
					rawName = Utils::sanitize(rawName);
					rawName = name + rawName;
					rawName = rawName.substr(0, rawName.find('\n'));
					if (rawName.length() < distance.length()) targetLen = DrawUtils::getTextWidth(&distance, 1) + 10.5;
					else targetLen = DrawUtils::getTextWidth(&rawName, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11);
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), true);
					DrawUtils::drawImage("textures/entity/steve.png", vec2_t(positionX + 5, positionY + 2), vec2_t(30, 30), vec2_t(0.125f, 0.125f), vec2_t(0.125f, 0.125f));
					vec4_t healthRect = vec4_t(testRect.x + 5, testRect.y + 38, testRect.x + ((targetLen - 4) / 20) * health, testRect.y + 40);
					DrawUtils::fillRoundRectangle(healthRect, MC_Color(0, 255, 0, 100), true);
				}

				if (mode.getSelectedValue() == 5) { // pulsive2
					float defaultRectHeight = (5.5, 5.5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					float opacity2 = (targetList[0]->damageTime * 80) / 8;
					string name = "          ";
					string rawName = targetList[0]->getNameTag()->getText();
					string fakename = "ABCDEFGHJIKLMNOPQ";
					auto damageTime = targetList[0]->damageTime;
					int smoothing = 3;
					int smoothing2 = 15;
					int maxsize = 30;
					int minsize = 27;
					if (damageTime > 1) {
						if (damageTime > 5) {
							if (size > minsize) size -= ((size - minsize) / smoothing);
							else if (size < minsize) size += ((minsize - size) / smoothing);
						}
						else {
							if (size > maxsize) size -= ((size - maxsize) / smoothing2);
							else if (size < maxsize) size += ((maxsize - size) / smoothing2);
						}
					}
					float size2 = size / 2;
					float sizepercent = size / 3;
					int redsize1 = 35 + positionX;
					int redsize2 = positionY + 32;
					rawName = fakename;
					rawName = Utils::sanitize(rawName);
					rawName = name + rawName;
					rawName = rawName.substr(0, rawName.find('\n'));
					if (rawName.length() < distance.length()) targetLen = DrawUtils::getTextWidth(&distance, 1) + 10.5;
					else targetLen = DrawUtils::getTextWidth(&rawName, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11);
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), true);
					//DrawUtils::drawRoundRectangle(testRect, interfaceColor, 150);
					DrawUtils::drawImage("textures/entity/steve.png", vec2_t(positionX + 20 - size2, positionY + 17 - size2), vec2_t(size, size), vec2_t(0.125f, 0.125f), vec2_t(0.125f, 0.125f));
					vec4_t healthRect = vec4_t(testRect.x + 5, testRect.y + 38, testRect.x + ((targetLen - 4) / 20) * displayhealth, testRect.y + 40);
					DrawUtils::fillRoundRectangle(healthRect, interfaceColor, 255);
					vec2_t healthTextPos = vec2_t(healthRect.x, healthRect.y + 1.f);
					//DrawUtils::drawText(healthTextPos, &displaypercent, MC_Color(255, 255, 255), 1, 1, true);
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(vec4_t(positionX + 20 - size2, positionY + 17 - size2, size + 6 + positionX, positionY + size + 3), MC_Color(255, 0, 0, steveOpacity));
					}
				}

				if (mode.getSelectedValue() == 8) { // Rise
					float defaultRectHeight = (5.5, 5.5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					float opacity2 = (targetList[0]->damageTime * 80) / 8;
					string name = "          ";
					string rawName = targetList[0]->getNameTag()->getText();
					string fakename = "ABCDEFGHJIKLMNOPQ";
					auto damageTime = targetList[0]->damageTime;
					int smoothing = 3;
					int smoothing2 = 15;
					int maxsize = 30;
					int minsize = 27;
					if (damageTime > 1) {
						if (damageTime > 5) {
							if (size > minsize) size -= ((size - minsize) / smoothing);
							else if (size < minsize) size += ((minsize - size) / smoothing);
						}
						else {
							if (size > maxsize) size -= ((size - maxsize) / smoothing2);
							else if (size < maxsize) size += ((maxsize - size) / smoothing2);
						}
					}
					float size2 = size / 2;
					float sizepercent = size / 3;
					int redsize1 = 35 + positionX;
					int redsize2 = positionY + 32;
					rawName = fakename;
					rawName = Utils::sanitize(rawName);
					rawName = name + rawName;
					rawName = rawName.substr(0, rawName.find('\n'));
					if (rawName.length() < distance.length()) targetLen = DrawUtils::getTextWidth(&distance, 1) + 10.5;
					else targetLen = DrawUtils::getTextWidth(&rawName, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, positionX + (targetLen * animationsize), positionY + ((defaultRectHeight - 11) * animationsize));
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), true);
					//DrawUtils::drawRoundRectangle(testRect, interfaceColor, 150);
					DrawUtils::drawImage("textures/entity/alex.png", vec2_t(positionX + 20 - size2, positionY + 17 - size2), vec2_t(size, size), vec2_t(0.125f, 0.125f), vec2_t(0.125f, 0.125f));
					vec4_t healthRect = vec4_t(testRect.x + 5, testRect.y + 38, testRect.x + ((targetLen - 4) / 20) * displayhealth, testRect.y + 40);
					//DrawUtils::fillRoundRectangle(healthRect, interfaceColor, 255);
					DrawUtils::fillRoundRectangleGradient(healthRect, RiseQuality);
					vec2_t healthTextPos = vec2_t(healthRect.x, healthRect.y + 1.f);
					//DrawUtils::drawText(healthTextPos, &displaypercent, MC_Color(255, 255, 255), 1, 1, true);
					//DrawUtils::drawText(healthTextPos, &positiontext, MC_Color(255, 255, 255), 1, 1, true);
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(vec4_t(positionX + 20 - size2, positionY + 17 - size2, size + 6 + positionX, positionY + size + 3), MC_Color(255, 0, 0, steveOpacity));
					}
				}

				if (mode.getSelectedValue() == 6) { // pulsive3
					float defaultRectHeight = (5.5, 5.5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					float opacity2 = (targetList[0]->damageTime * 80) / 8;
					string name = "          ";
					string rawName = targetList[0]->getNameTag()->getText();
					string fakename = "ABCDEFGHJIKLMNOPQ";
					auto damageTime = targetList[0]->damageTime;
					int smoothing = 5;
					int maxsize = 30;
					int minsize = 27;
					if (damageTime > 1) {
						if (damageTime > 5) {
							if (size > minsize) size -= ((size - minsize) / smoothing);
							else if (size < minsize) size += ((minsize - size) / smoothing);
						}
						else {
							if (size > maxsize) size -= ((size - maxsize) / smoothing);
							else if (size < maxsize) size += ((maxsize - size) / smoothing);
						}
					}
					float size2 = size / 30;
					rawName = fakename;
					rawName = Utils::sanitize(rawName);
					rawName = name + rawName;
					rawName = rawName.substr(0, rawName.find('\n'));
					if (rawName.length() < distance.length()) targetLen = DrawUtils::getTextWidth(&distance, 1) + 10.5;
					else targetLen = DrawUtils::getTextWidth(&rawName, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11);
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), true);
					DrawUtils::drawRoundRectangle(testRect, interfaceColor, 150);
					/*if (targetList[0]->damageTime > 1) {
						DrawUtils::drawImage("textures/entity/steve.png", vec2_t(positionX + 5 / size2 * 1.0f, positionY + 2 / size2 * 1.0f), vec2_t(size, size), vec2_t(0.125f, 0.125f), vec2_t(0.125f, 0.125f));
					}
					else
					{
						DrawUtils::drawImage("textures/entity/steve.png", vec2_t(positionX + 5, positionY + 2), vec2_t(30, 30), vec2_t(0.125f, 0.125f), vec2_t(0.125f, 0.125f));
					}*/
					DrawUtils::drawOldSteve(vec4_t(positionX + 5.f, positionY + 2.f, positionX, positionY));
					vec4_t healthRect = vec4_t(testRect.x + 5, testRect.y + 38, testRect.x + ((targetLen - 4) / 20) * displayhealth, testRect.y + 40);
					DrawUtils::fillRoundRectangle(healthRect, interfaceColor, 255);
					//DrawUtils::drawRoundRectangle(healthRect, MC_Color(200, 200, 200), 150);
					vec2_t healthTextPos = vec2_t(healthRect.x, healthRect.y + 1.f);
					//DrawUtils::drawText(healthTextPos, &displaypercent, MC_Color(255, 255, 255), 1, 1, true);
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(vec4_t(positionX + 5, positionY + 2, 35 + positionX, positionY + 32), MC_Color(255, 0, 0, steveOpacity));
					}
					//DrawUtils::drawRoundRectangle(vec4_t(positionX + 5, positionY + 2, 35 + positionX, positionY + 32), MC_Color(255, 0, 0, steveOpacity), true);
				}

				if (mode.getSelectedValue() == 7) { // pulsive4.0
					float defaultRectHeight = (5.5, 5.5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					float opacity2 = (targetList[0]->damageTime * 80) / 8;
					string name = "          ";
					string rawName = targetList[0]->getNameTag()->getText();
					string fakename = "ABCDEFGHJIKLMNOPQ";
					auto damageTime = targetList[0]->damageTime;
					int smoothing = 5;
					int maxsize = 30;
					int minsize = 27;
					if (damageTime > 1) {
						if (damageTime > 5) {
							if (size > minsize) size -= ((size - minsize) / smoothing);
							else if (size < minsize) size += ((minsize - size) / smoothing);
						}
						else {
							if (size > maxsize) size -= ((size - maxsize) / smoothing);
							else if (size < maxsize) size += ((maxsize - size) / smoothing);
						}
					}
					float size2 = size / 30;
					rawName = fakename;
					rawName = Utils::sanitize(rawName);
					rawName = name + rawName;
					rawName = rawName.substr(0, rawName.find('\n'));
					if (rawName.length() < distance.length()) targetLen = DrawUtils::getTextWidth(&distance, 1) + 10.5;
					else targetLen = DrawUtils::getTextWidth(&rawName, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 19);
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), true);
					DrawUtils::drawRoundRectangle(testRect, interfaceColor, 150);
					DrawUtils::drawOldSteve(vec4_t(positionX + 5.f, positionY + 2.f, positionX, positionY));
					vec4_t healthRect = vec4_t(testRect.x + 40, testRect.y + 23, testRect.x + ((targetLen - 4) / 20) * displayhealth, testRect.y + 30);
					vec4_t BlackRect = vec4_t(testRect.x + 40, testRect.y + 23, testRect.x + ((targetLen - 4) / 20) * 20, testRect.y + 30);
					DrawUtils::fillRoundRectangle(BlackRect, MC_Color(0, 0, 0, 255), true);
					DrawUtils::fillRoundRectangle(healthRect, interfaceColor, 255);
					vec2_t healthTextPos = vec2_t(healthRect.x, healthRect.y - 10.f);
					DrawUtils::drawText(healthTextPos, &smoothtext, MC_Color(255, 255, 255), 0.7, 0.7, true);
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(vec4_t(positionX + 5, positionY + 2, 35 + positionX, positionY + 32), MC_Color(255, 0, 0, steveOpacity));
					}
					//DrawUtils::drawRoundRectangle(vec4_t(positionX + 5, positionY + 2, 35 + positionX, positionY + 32), MC_Color(255, 0, 0, steveOpacity), true);
				}

				if (mode.getSelectedValue() == 3) { // novoline
					float targetLen = 37.f + DrawUtils::getTextWidth(&targetName, 1);
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + novolineRectHeight);
					DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), false);
					vec4_t healthRect = vec4_t(testRect.x + 35, testRect.y + 20, testRect.x + ((targetLen - 4) / 20) * health, testRect.y + 30);
					vec2_t healthTextPos = vec2_t(healthRect.x, healthRect.y + 1.f);
					DrawUtils::drawText(healthTextPos, &percent, MC_Color(255, 255, 255), 1, 1, true);
					float opacity = (targetList[0]->damageTime * 80) / 8;
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(healthRect, MC_Color(255, 0, 0, opacity));
					}
					else {
						DrawUtils::fillRectangleA(healthRect, MC_Color(0, 255, 0, 90));
					}
					DrawUtils::drawSteve(vec4_t(positionX + 2.f, positionY + 1.f, positionX, positionY));
					if (targetList[0]->damageTime > 1) {
						DrawUtils::fillRectangleA(vec4_t(positionX + 2.f, positionY, 32.f + positionX, positionY + 32.f), MC_Color(255, 0, 0, opacity));
					}
				}


				if (mode.getSelectedValue() == 1) {
					if (showItems) defaultRectHeight = (7, 2) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					string name = targetList[0]->getNameTag()->getText();
					name = Utils::sanitize(name);
					string namestr = "Name: " + name;
					namestr = namestr.substr(0, namestr.find('\n'));
					if (namestr.length() < position.length()) targetLen = DrawUtils::getTextWidth(&position, 1) + 6.5;
					else targetLen = DrawUtils::getTextWidth(&namestr, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 10);
					if (showItems) testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight * 2.5f);
					DrawUtils::fillRectangleA(testRect, MC_Color(0, 0, 0, opacity));
					vec4_t healthRect = vec4_t(testRect.x + 4, testRect.y + 30, testRect.x + targetLen - 4, testRect.y + 35);
					if (showItems) healthRect = vec4_t(testRect.x + 4, testRect.y + 40, testRect.x + targetLen - 4, testRect.y + 45);
					float opacity = (targetList[0]->damageTime * 80) / 8;
					if (targetList[0]->damageTime > 1) {
						DrawUtils::drawRectangle(healthRect, MC_Color(255, 0, 0), 0.3);
						DrawUtils::fillRectangleA(healthRect, MC_Color(255, 0, 0, 90));
					}
					else {
						DrawUtils::drawRectangle(healthRect, MC_Color(0, 255, 0), 0.3);
						DrawUtils::fillRectangleA(healthRect, MC_Color(0, 255, 0, 90));
					}
				}
			}

			for (auto& i : targetList) {
				if (mode.getSelectedValue() == 0) {
					string namestr = string(GRAY) + "Name: " + string(RESET) + targetName;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight);
					vec2_t namePos = vec2_t(testRect.x + 39, testRect.y + 5);
					vec2_t distPos = vec2_t(testRect.x + 39, testRect.y + 15);
					vec2_t armorPos = vec2_t(testRect.x + 39, testRect.y + 25);
					DrawUtils::drawText(distPos, &distance, MC_Color(255, 255, 255), 1, 1, true);
					DrawUtils::drawText(namePos, &namestr, MC_Color(255, 255, 255), 1, 1, true);
					if (i->getEntityTypeId() == 319 && showItems) {
						static float constexpr opacity = 10;
						float scale = 3 * 0.26f;
						float spacing = scale + 15.f + 2;
						auto* player = reinterpret_cast<C_Player*>(targetList[0]);
						for (int t = 0; t < 4; t++) {
							C_ItemStack* stack = player->getArmor(t);
							if (stack->isValid()) {
								DrawUtils::drawItem(stack, vec2_t(armorPos), 1, scale, false);
								armorPos.x += scale * spacing;
							}
						}
						C_PlayerInventoryProxy* supplies = player->getSupplies();
						C_ItemStack* item = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);
						if (item->isValid()) DrawUtils::drawItem(item, vec2_t(armorPos), opacity, scale, item->isEnchanted());
					}
				}

				if (mode.getSelectedValue() == 3) {
					sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
					float targetLen = DrawUtils::getTextWidth(&targetName, 1) + 35.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + novolineRectHeight);
					DrawUtils::drawText(vec2_t(testRect.x + 35, testRect.y + 5), &targetName, MC_Color(255, 255, 255), 1, 1, true);
				}

				if (mode.getSelectedValue() == 4) {
					sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
					float targetLen = DrawUtils::getTextWidth(&targetName, 1) + 35.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11.f);
					DrawUtils::drawText(vec2_t(testRect.x + 40, testRect.y + 15), &targetName, MC_Color(255, 255, 255), 1, 1, true);
				}

				if (mode.getSelectedValue() == 5 || mode.getSelectedValue() == 6) {
					sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
					float targetLen = DrawUtils::getTextWidth(&targetName, 1) + 35.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11.f);
					DrawUtils::drawText(vec2_t(testRect.x + 40, testRect.y + 15), &targetName, MC_Color(255, 255, 255), 1, 1, true);
				}

				if (mode.getSelectedValue() == 7) {
					sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
					float targetLen = DrawUtils::getTextWidth(&targetName, 1) + 35.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11.f);
					DrawUtils::drawText(vec2_t(testRect.x + 40, testRect.y + 5), &targetName, MC_Color(255, 255, 255), 1, 1, true);
				}

				if (mode.getSelectedValue() == 8) {
					sort(targetList.begin(), targetList.end(), CompareTargetEnArray());
					float targetLen = DrawUtils::getTextWidth(&targetName, 1) + 35.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11.f);
					DrawUtils::drawText(vec2_t(testRect.x + 40, testRect.y + 15), &targetName, MC_Color(255, 255, 255), 1, 1, true);
				}

				// Old
				if (mode.getSelectedValue() == 1) {
					string name = targetList[0]->getNameTag()->getText();
					name = Utils::sanitize(name);
					string namestr = "Name: " + name;
					namestr = namestr.substr(0, namestr.find('\n'));
					static const float rectHeight = (5, 5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
					if (namestr.length() < position.length()) targetLen = DrawUtils::getTextWidth(&position, 1) + 6.5;
					else targetLen = DrawUtils::getTextWidth(&namestr, 1) + 6.5;
					vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + rectHeight);
					vec2_t namePos = vec2_t(testRect.x + 4, testRect.y + 5);
					vec2_t distPos = vec2_t(testRect.x + 4, testRect.y + 15);
					vec2_t armorPos = vec2_t(testRect.x + 4, testRect.y + 25);
					DrawUtils::drawText(distPos, &position, MC_Color(255, 255, 255), 1, 1, true);
					DrawUtils::drawText(namePos, &namestr, MC_Color(255, 255, 255), 1, 1, true);
					if (i->getEntityTypeId() == 319 && showItems) {
						static float constexpr opacity = 10;
						float scale = 3 * 0.26f;
						float spacing = scale + 15.f + 2;
						auto* player = reinterpret_cast<C_Player*>(targetList[0]);
						for (int t = 0; t < 4; t++) {
							C_ItemStack* stack = player->getArmor(t);
							if (stack->isValid()) {
								DrawUtils::drawItem(stack, vec2_t(armorPos), 1, scale, false);
								armorPos.x += scale * spacing;
							}
						}
						C_PlayerInventoryProxy* supplies = player->getSupplies();
						C_ItemStack* item = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);
						if (item->isValid()) DrawUtils::drawItem(item, vec2_t(armorPos), opacity, scale, item->isEnchanted());
					}
				}

				// Simple
				if (mode.getSelectedValue() == 2) {
					// Simple
					if (mode.getSelectedValue() == 2) {
						static const float rectHeight = (5, 5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
						float targetLen = 37.f + DrawUtils::getTextWidth(&targetName, 1);
						vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + rectHeight);
						vec2_t namePos = vec2_t(testRect.x + 4, testRect.y + 5);
						vec2_t distPos = vec2_t(testRect.x + 4, testRect.y + 15);
						vec2_t armorPos = vec2_t(testRect.x + 4, testRect.y + 25);
						DrawUtils::drawText(distPos, &distance, MC_Color(255, 255, 255), 1, 1, true);
						DrawUtils::drawText(namePos, &targetName, MC_Color(255, 255, 255), 1, 1, true);
						if (i->getEntityTypeId() == 319 && showItems) {
							static float constexpr opacity = 10;
							float scale = 3 * 0.26f;
							float spacing = scale + 15.f + 2;
							auto* player = reinterpret_cast<C_Player*>(targetList[0]);
							for (int t = 0; t < 4; t++) {
								C_ItemStack* stack = player->getArmor(t);
								if (stack->isValid()) {
									DrawUtils::drawItem(stack, vec2_t(armorPos), 1, scale, false);
									armorPos.x += scale * spacing;
								}
							}
							C_PlayerInventoryProxy* supplies = player->getSupplies();
							C_ItemStack* item = supplies->inventory->getItemStack(supplies->selectedHotbarSlot);
							if (item->isValid()) DrawUtils::drawItem(item, vec2_t(armorPos), opacity, scale, item->isEnchanted());
						}
					}
				}
			}
		}

		if (clickGUI->isEnabled() && !clickGUI->hasOpenedGUI && targetList.size() == 0) { // if theres no targets use local player
			auto absorbtion = 0.f/*g_Data.getLocalPlayer()->getAttribute(&attribute)->currentValue*/;
			float dist = 0;
			float defaultRectHeight = (5, 5) * DrawUtils::getFont(Fonts::SMOOTH)->getLineHeight();
			string distance = string(GRAY) + "Distance: " + string(RESET) + to_string((int)dist) + string(".") + to_string((int)(dist * 10) - ((int)dist * 10));
			string name = string(GRAY) + "Name: " + string(RESET);
			string rawName = g_Data.getLocalPlayer()->getNameTag()->getText();
			rawName = Utils::sanitize(rawName);
			string nlelel = string(GRAY) + "Name:          " + string(RESET) + rawName;
			rawName = name + rawName; rawName = rawName.substr(0, rawName.find('\n'));
			targetLen = DrawUtils::getTextWidth(&nlelel, 1) + 6.5;
			vec4_t testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight - 11);
			if (showItems) testRect = vec4_t(positionX, positionY, targetLen + positionX, positionY + defaultRectHeight);
			DrawUtils::fillRoundRectangle(testRect, MC_Color(0, 0, 0, opacity), false);
			vec4_t healthRect = vec4_t(testRect.x + 39, testRect.y + 32, testRect.x + ((targetLen - 4) / 20) * 20, testRect.y + 34);
			if (showItems) healthRect = vec4_t(testRect.x + 4, testRect.y + 44, testRect.x + ((targetLen - 4) / 20) * 20, testRect.y + 47);
			vec2_t namePos = vec2_t(testRect.x + 39, testRect.y + 5);
			vec2_t distPos = vec2_t(testRect.x + 39, testRect.y + 15);
			DrawUtils::drawText(distPos, &distance, MC_Color(255, 255, 255), 1, 1, true);
			DrawUtils::drawText(namePos, &rawName, MC_Color(255, 255, 255), 1, 1, true);
			DrawUtils::drawRectangle(healthRect, MC_Color(0, 255, 0), 0.3);
			DrawUtils::fillRectangleA(healthRect, MC_Color(0, 255, 0, 90));
			vec4_t absorbtionRect = vec4_t(testRect.x + 39, testRect.y + 26, testRect.x + absorbtion * 4.f, testRect.y + 29);
			if (showItems) absorbtionRect = vec4_t(testRect.x + 4, testRect.y + 38, testRect.x + absorbtion * 4.f, testRect.y + 41);
			vec4_t absorbtionSubRect = vec4_t(testRect.x + 39, testRect.y + 28, testRect.x + targetLen - 4.f, testRect.y + 30);
			if (showItems) absorbtionSubRect = vec4_t(testRect.x + 4, testRect.y + 38, testRect.x + targetLen - 4.f, testRect.y + 41);
			DrawUtils::fillRectangleA(absorbtionSubRect, MC_Color(25, 25, 25, 255));
			if (absorbtion > 1.f) {
				DrawUtils::drawRectangle(absorbtionRect, MC_Color(255, 230, 0), 0.3);
				DrawUtils::fillRectangleA(absorbtionRect, MC_Color(255, 230, 0, 255));
			}
			DrawUtils::drawSteve(vec4_t(positionX + 5, positionY + 5, positionX, positionY + 20));
		}
	}
}