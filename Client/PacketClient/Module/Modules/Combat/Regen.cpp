#include "Regen.h"
#include "../../../../SDK/CAttribute.h"
#include "../pch.h"

using namespace std;
Regen::Regen() : IModule(0, Category::COMBAT, "Regenerates your health") {
	registerEnumSetting("Mode", &mode, 0);
	mode.addEntry("Vanilla", 0);
	mode.addEntry("Hive", 1);
	mode.addEntry("HiveDelayed", 2);
	mode.addEntry("HiveMulti", 3);
	mode.addEntry("HiveSingle", 4);
	mode.addEntry("HiveSingle+", 5);
	mode.addEntry("FlareonTest", 6);
	registerFloatSetting("Range", &range, range, 1, 7);
	registerIntSetting("Delay", &delay, delay, 0, 20);
	registerBoolSetting("Rotation", &rotation, rotation);
	registerBoolSetting("Swing", &swing, swing);
	registerBoolSetting("SmoothRot", &smooth, smooth);
}

const char* Regen::getRawModuleName() {
	return "Regen";
}

const char* Regen::getModuleName() {
	if (mode.getSelectedValue() == 0) name = string("Regen ") + string(GRAY) + string("Vanilla");
	if (mode.getSelectedValue() == 1) name = string("Regen ") + string(GRAY) + string("Hive");
	if (mode.getSelectedValue() == 2) name = string("Regen ") + string(GRAY) + string("HiveDelayed");
	if (mode.getSelectedValue() == 3) name = string("Regen ") + string(GRAY) + string("HiveMulti");
	if (mode.getSelectedValue() == 4) name = string("Regen ") + string(GRAY) + string("HiveSingle");
	if (mode.getSelectedValue() == 5) name = string("Regen ") + string(GRAY) + string("HiveSingle+");
	if (mode.getSelectedValue() == 6) name = string("Regen ") + string(GRAY) + string("FlareonTest");
	return name.c_str();
}

bool Regen::selectPickaxe() {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	auto prevSlot = supplies->selectedHotbarSlot;

	for (int n = 0; n < 36; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr) {
			if (stack->getItem()->isPickaxe()) {
				if (prevSlot != n) {
					supplies->selectedHotbarSlot = n;
				}
				return true;
			}
		}
	}
	return false;
}

void Regen::onEnable() {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	rot = false;
	bypassrot = false;
	firstbreak = false;
	continuemine = false;
	canattack = true;
	isregen = true;
	enabledticks = 0;
	blocksBroke = 0;

	animYawB = player->yawUnused1;
	animPitchB = player->pitch;

	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	slot = supplies->selectedHotbarSlot;
}

void Regen::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr || moduleMgr->getModule<Scaffold>()->isEnabled()) return;
	//int health = player->getHealth();
	/*if (HealthLimitter && health > limithealth) {
		isregen = false;
		return;
	}else isregen = true;*/
	tick++;
	enabledticks++;

	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	static AbsorptionAttribute attribute = AbsorptionAttribute();
	auto timerUtil = new TimerUtil();
	static vector<vec3_ti> blocks;

	if (tick == delay) tick = 0;
	if (g_Data.canUseMoveKeys()) {
		switch (mode.getSelectedValue()) {
		case 0: // Vanilla
			// nothing lol
			break;
		case 1: // Hive
			if (blocks.empty()) {
				for (int x = -range; x <= range; x++) {
					for (int z = -range; z <= range; z++) {
						for (int y = -range; y <= range; y++) {
							blocks.push_back(vec3_ti(x, y, z));
						}
					}
				}
				// https://www.mathsisfun.com/geometry/pythagoras-3d.html sort = a12 + a22 + √(x2 + y2 + z2) + an2 √(a12 + a22 + √(x2 + y2 + z2) + an2) / 4.f
				sort(blocks.begin(), blocks.end(), [](vec3_ti start, vec3_ti end) {
					return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
					});
			}

			for (const vec3_ti& offset : blocks) {
				blockPos = gm->player->getPos()->add(offset.toVector3());
				destroy = false;

				string name = gm->player->region->getBlock(blockPos)->toLegacy()->name.getText();
				if (name == "lit_redstone_ore" || name == "redstone_ore"/*player->getAttribute(&attribute)->currentvalue*/) {
					destroy = true;
				}


				int aids = delay - 1;
				if (destroy && tick >= aids) {
					slot = supplies->selectedHotbarSlot;
					static bool restored = false;
					if (!selectPickaxe()) {
						if (!restored) restored = true;
						else {
							auto warning = g_Data.addNotification("Regen", "No Pickaxe Found"); warning->duration = 3;
							setEnabled(false);
						}
						return;
					}
					bool isDestroyed = false;
					restored = false;
					if (firstbreak) gm->startDestroyBlock(blockPos, 1, isDestroyed);
					tempblockPos = gm->player->getPos()->add(offset.toVector3());
					rot = true;
					if (firstbreak) {
						gm->destroyBlock(&blockPos, 0); hasDestroyed = true; rot = false;
					}
					if (firstbreak) {
						firstbreak = false;
					}
					else firstbreak = true;
					supplies->selectedHotbarSlot = slot;
					if (!swing) player->swing();
					return;
				}

				if (destroy) {
					bool isDestroyed = false;
					gm->startDestroyBlock(blockPos, 1, isDestroyed);
				}

				if (hasDestroyed) { hasDestroyed = false; blocksBroke += 1; }
				if (blocksBroke > 5) blocksBroke = 0;
			}
			break;
		case 2: // HiveMulti Delayed
			if (blocks.empty()) {
				for (int x = -range; x <= range; x++) {
					for (int z = -range; z <= range; z++) {
						for (int y = -range; y <= range; y++) {
							blocks.push_back(vec3_ti(x, y, z));
						}
					}
				}
				// https://www.mathsisfun.com/geometry/pythagoras-3d.html sort = a12 + a22 + √(x2 + y2 + z2) + an2 √(a12 + a22 + √(x2 + y2 + z2) + an2) / 4.f
				sort(blocks.begin(), blocks.end(), [](vec3_ti start, vec3_ti end) {
					return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
					});
			}

			for (const vec3_ti& offset : blocks) {
				blockPos = gm->player->getPos()->add(offset.toVector3());
				tempblockPos = gm->player->getPos()->add(offset.toVector3());
				destroy = false;

				string name = gm->player->region->getBlock(blockPos)->toLegacy()->name.getText();
				if (name == "lit_redstone_ore" || name == "redstone_ore" /* && attribute->getCurrentHealth() < 10*/) {
					destroy = true;
				}


				int aids = delay - 1;
				if (destroy && tick >= aids) {
					if (enabledticks > 10) {
						slot = supplies->selectedHotbarSlot;
						static bool restored = false;
						if (!selectPickaxe()) {
							if (!restored) restored = true;
							else {
								auto warning = g_Data.addNotification("Regen", "No Pickaxe Found"); warning->duration = 3;
								setEnabled(false);
							}
							return;
						}
						bool isDestroyed = false;
						restored = false;
						if (firstbreak) gm->startDestroyBlock(blockPos, 1, isDestroyed);
						tempblockPos = gm->player->getPos()->add(offset.toVector3());
						rot = true;
						if (firstbreak && enabledticks > 10) {
							gm->destroyBlock(&blockPos, 0); hasDestroyed = true; rot = false;
						}
						if (firstbreak) {
							firstbreak = false;
						}
						else firstbreak = true;
						supplies->selectedHotbarSlot = slot;
						if (!swing) player->swing();
						return;
					}
				}

				if (destroy) {
					bool isDestroyed = false;
					gm->startDestroyBlock(blockPos, 1, isDestroyed);
				}

				if (hasDestroyed) { hasDestroyed = false; blocksBroke += 1; }
				if (blocksBroke > 5) blocksBroke = 0;
			}
			break;
		case 3: // HiveMulti
			smooth = false;
			delay = 1;

			//Switch Killaura
			if (enabledticks > 10) {
				isregen = true;
			}
			else
			{
				isregen = false;
			}

			if (blocks.empty()) {
				for (int x = -range; x <= range; x++) {
					for (int z = -range; z <= range; z++) {
						for (int y = -range; y <= range; y++) {
							blocks.push_back(vec3_ti(x, y, z));
						}
					}
				}
				// https://www.mathsisfun.com/geometry/pythagoras-3d.html sort = a12 + a22 + √(x2 + y2 + z2) + an2 √(a12 + a22 + √(x2 + y2 + z2) + an2) / 4.f
				sort(blocks.begin(), blocks.end(), [](vec3_ti start, vec3_ti end) {
					return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
					});
			}

			for (const vec3_ti& offset : blocks) {
				blockPos = gm->player->getPos()->add(offset.toVector3());
				tempblockPos = gm->player->getPos()->add(offset.toVector3());
				destroy = false;

				string name = gm->player->region->getBlock(blockPos)->toLegacy()->name.getText();
				if (name == "lit_redstone_ore" || name == "redstone_ore" /* && attribute->getCurrentHealth() < 10*/) {
					destroy = true;
					/*if (rotation) {
						vec2_t regenRot = player->getPos()->CalcAngle(vec3_t(tempblockPos));
						player->yawUnused1 = regenRot.y;
						player->pitch = regenRot.x;
						player->bodyYaw = regenRot.y;
					}*/
				}

				bool isDestroyed = false;
				if (destroy) {
					gm->startDestroyBlock(blockPos, 1, isDestroyed);
				}

				int aids = delay - 1;
				if (destroy && tick >= aids) {
					if (enabledticks > 10) {
						slot = supplies->selectedHotbarSlot;
						static bool restored = false;
						if (!selectPickaxe()) {
							if (!restored) restored = true;
							else {
								auto warning = g_Data.addNotification("Regen", "No Pickaxe Found"); warning->duration = 3;
								setEnabled(false);
							}
							return;
						}
						restored = false;
						//if (firstbreak) gm->startDestroyBlock(blockPos, 1, isDestroyed);
						tempblockPos = gm->player->getPos()->add(offset.toVector3());
						rot = true;
						if (firstbreak && enabledticks > 10 && rot) {
							gm->destroyBlock(&blockPos, 0); hasDestroyed = true; rot = false;
							gm->stopDestroyBlock(blockPos);
						}
						if (firstbreak) {
							firstbreak = false;
						}
						else {
							firstbreak = true;
							vec2_t regenRot = player->getPos()->CalcAngle(vec3_t(tempblockPos));
							animYawB = player->yawUnused1;
							animPitchB = player->pitch;
							if (animYawB > regenRot.y) animYawB -= ((animYawB - regenRot.y) / 1.8);
							else if (animYawB < regenRot.y) animYawB += ((regenRot.y - animYawB) / 1.8);

							if (animPitchB > regenRot.x) animPitchB -= ((animPitchB - regenRot.x) / 1.8);
							else if (animPitchB < regenRot.x) animPitchB += ((regenRot.x - animPitchB) / 1.8);
							player->yawUnused1 = animYawB;
							player->pitch = animPitchB;
							player->bodyYaw = animYawB;

						}
						supplies->selectedHotbarSlot = slot;
						if (!swing) player->swing();
						return;
					}
				}

				if (hasDestroyed) { hasDestroyed = false; blocksBroke += 1; }
				if (blocksBroke > 4) { blocksBroke = 0; setEnabled(false); }
			}
			break;
		case 4: // FlareonSingle
		{
			//Check Absorpription Health Check
/*if (gm->player->getAttribute(&attribute)->currentValue < 10) {
	isregen = false;
	return;
}
else
{
	isregen = true;
}*/

//Switch Killaura
			if (enabledticks > 12) {
				isregen = true;
			}
			else
			{
				isregen = false;
				animYawB = player->yawUnused1;
				animPitchB = player->pitch;
			}

			if (blocks.empty()) {
				for (int x = -range; x <= range; x++) {
					for (int z = -range; z <= range; z++) {
						for (int y = -range; y <= range; y++) {
							blocks.push_back(vec3_ti(x, y, z));
						}
					}
				}
				// https://www.mathsisfun.com/geometry/pythagoras-3d.html sort = a12 + a22 + √(x2 + y2 + z2) + an2 √(a12 + a22 + √(x2 + y2 + z2) + an2) / 4.f
				sort(blocks.begin(), blocks.end(), [](vec3_ti start, vec3_ti end) {
					return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
					});
			}

			bool isDestroyed = false;

			for (const vec3_ti& offset : blocks) {
				blockPos = gm->player->getPos()->add(offset.toVector3());
				vec3_t blockPos2 = gm->player->getPos()->add(offset.toVector3());
				destroy = false;

				string name = gm->player->region->getBlock(blockPos)->toLegacy()->name.getText();
				if (continuemine) {
					if (name == "lit_redstone_ore" || name == "redstone_ore" && tempblockPos == blockPos2) {
						destroy = true;
						//selectPickaxe();
						//gm->startDestroyBlock(blockPos, 1, isDestroyed);
						tempblockPos = blockPos2; // moved
					}
				}
				else {
					if (name == "lit_redstone_ore" || name == "redstone_ore"/*player->getAttribute(&attribute)->currentvalue*/) {
						destroy = true;
						//selectPickaxe();
						gm->startDestroyBlock(blockPos, 1, isDestroyed);
						continuemine = true;
						tempblockPos = blockPos2;
					}
				}

				/*if (!destroy) {
					enabledticks = 0;
				}*/


				int aids = delay - 1;
				if (destroy/* && tick >= aids*/) {
					if (swing) player->swing();
					slot = supplies->selectedHotbarSlot;
					static bool restored = false;
					if (!selectPickaxe()) {
						if (!restored) restored = true;
						else {
							auto warning = g_Data.addNotification("Regen", "No Pickaxe Found"); warning->duration = 3;
							setEnabled(false);
						}
						return;
					}
					restored = false;
					if (isregen) rot = true;
					/*if (enabledticks > 2 && tempblockPos != OldBlockPos) {
						gm->stopDestroyBlock(OldBlockPos);
						enabledticks = 0;
						clientMessageF("BlockChanged");
					} */
					//if (enabledticks > 12 && isregen) clientMessageF("IsDestroyed");
					if (firstbreak && enabledticks > 16) {
						gm->destroyBlock(&blockPos, 0); hasDestroyed = true; rot = false; enabledticks = 0; gm->stopDestroyBlock(blockPos); continuemine = false;
					}
					if (firstbreak) {
						firstbreak = false;
					}
					else firstbreak = true;
					supplies->selectedHotbarSlot = slot;
					if (!swing) player->swing();
					return;
				}

				if (hasDestroyed) { hasDestroyed = false; blocksBroke += 1; }
				if (blocksBroke > 4) blocksBroke = 0;

				//OldBlockPos = gm->player->getPos()->add(offset.toVector3());
			}

			if (!destroy) {
				//clientMessageF("Oof redstone not found");
				isregen = false;
				continuemine = false;
				canattack = true;
				enabledticks = 0;
				//clientMessageF("Lost Redstone");
			}
			else
			{
				canattack = true;
			}
		}
		break;
		case 5: // FlareonSingle+
		{

			//Check Absorpription Health Check
			if (gm->player->getMutableAttribute(&attribute)->currentValue >= 10) {
				isregen = false;
				continuemine = false;
				canattack = true;
				enabledticks = 0;
				rot = false;
				return;
			}
			else
			{
				isregen = true;
			}

			if (enabledticks > 12) {
				isregen = true;
			}
			else
			{
				isregen = false;
				canattack = false;
				animYawB = player->yawUnused1;
				animPitchB = player->pitch;
			}

			if (blocks.empty()) {
				for (int x = -range; x <= range; x++) {
					for (int z = -range; z <= range; z++) {
						for (int y = -range; y <= range; y++) {
							blocks.push_back(vec3_ti(x, y, z));
						}
					}
				}
				// https://www.mathsisfun.com/geometry/pythagoras-3d.html sort = a12 + a22 + √(x2 + y2 + z2) + an2 √(a12 + a22 + √(x2 + y2 + z2) + an2) / 4.f
				sort(blocks.begin(), blocks.end(), [](vec3_ti start, vec3_ti end) {
					return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
					});
			}

			bool isDestroyed2 = false;

			for (const vec3_ti& offset : blocks) {
				blockPos = gm->player->getPos()->add(offset.toVector3());
				vec3_t blockPos2 = gm->player->getPos()->add(offset.toVector3());
				destroy = false;

				string name = gm->player->region->getBlock(blockPos)->toLegacy()->name.getText();
				vec3_ti AirCheckPos;
				bool isSafe = false;
				AirCheckPos = blockPos;
				//if (gm->player->region->getBlock(AirCheckPos.add(0, 1, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 1, 0))->toLegacy()->material->isReplaceable) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, -1, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(1, 0, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(-1, 0, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 0, 1))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 0, -1))->toLegacy()->blockId == 0) isSafe = true;
				if (continuemine) {
					if (name == "lit_redstone_ore" || name == "redstone_ore" && tempblockPos == blockPos2 && isSafe) {
						destroy = true;
						//selectPickaxe();
						//gm->startDestroyBlock(blockPos, 1, isDestroyed);
						tempblockPos = blockPos2; // moved
					}
				}
				else {
					if (name == "lit_redstone_ore" || name == "redstone_ore" && isSafe) {
						destroy = true;
						//selectPickaxe();
						gm->startDestroyBlock(blockPos, 1, isDestroyed2);
						continuemine = true;
						tempblockPos = blockPos2;
					}
				}

				/*if (!destroy) {
					enabledticks = 0;
				}*/


				int aids = delay - 1;
				if (destroy/* && tick >= aids*/) {
					if (swing) player->swing();
					slot = supplies->selectedHotbarSlot;
					static bool restored = false;
					if (!selectPickaxe()) {
						if (!restored) restored = true;
						else {
							auto warning = g_Data.addNotification("Regen", "No Pickaxe Found"); warning->duration = 3;
							setEnabled(false);
						}
						return;
					}
					restored = false;
					if (isregen) rot = true;
					/*if (enabledticks > 2 && tempblockPos != OldBlockPos) {
						gm->stopDestroyBlock(OldBlockPos);
						enabledticks = 0;
						clientMessageF("BlockChanged");
					} */
					//if (enabledticks > 12 && isregen) clientMessageF("IsDestroyed");
					if (firstbreak && enabledticks > 16) {
						gm->destroyBlock(&blockPos, 0); hasDestroyed = true; rot = false; enabledticks = 0; gm->stopDestroyBlock(blockPos); continuemine = false;
					}
					if (firstbreak) {
						firstbreak = false;
					}
					else firstbreak = true;
					supplies->selectedHotbarSlot = slot;
					if (!swing) player->swing();
					return;
				}

				if (hasDestroyed) { hasDestroyed = false; blocksBroke += 1; }
				if (blocksBroke > 4) blocksBroke = 0;

				//OldBlockPos = gm->player->getPos()->add(offset.toVector3());
			}

			if (!destroy) {
				//clientMessageF("Oof redstone not found");
				isregen = false;
				continuemine = false;
				canattack = true;
				enabledticks = 0;
				rot = false;
				//clientMessageF("Lost Redstone");
			}
			else
			{
				canattack = true;
			}
		}
		break;
		case 6: // FlareonTest
		{
			smooth = false;
			//Check Absorpription Health Check
			if (gm->player->getMutableAttribute(&attribute)->currentValue >= 10) {
				isregen = false;
				continuemine = false;
				canattack = true;
				enabledticks = 0;
				rot = false;
				return;
			}
			else
			{
				isregen = true;
			}

			if (enabledticks > 15) {
				isregen = true;
			}
			else
			{
				isregen = false;
				canattack = false;
				animYawB = player->yawUnused1;
				animPitchB = player->pitch;
			}

			if (blocks.empty()) {
				for (int x = -range; x <= range; x++) {
					for (int z = -range; z <= range; z++) {
						for (int y = -range; y <= range; y++) {
							blocks.push_back(vec3_ti(x, y, z));
						}
					}
				}
				// https://www.mathsisfun.com/geometry/pythagoras-3d.html sort = a12 + a22 + √(x2 + y2 + z2) + an2 √(a12 + a22 + √(x2 + y2 + z2) + an2) / 4.f
				sort(blocks.begin(), blocks.end(), [](vec3_ti start, vec3_ti end) {
					return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
					});
			}

			bool isDestroyed2 = false;

			for (const vec3_ti& offset : blocks) {
				blockPos = gm->player->getPos()->add(offset.toVector3());
				vec3_t blockPos2 = gm->player->getPos()->add(offset.toVector3());
				destroy = false;

				string name = gm->player->region->getBlock(blockPos)->toLegacy()->name.getText();
				vec3_ti AirCheckPos;
				bool isSafe = false;
				AirCheckPos = blockPos;
				/*if (gm->player->region->getBlock(AirCheckPos.add(0, 1, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, -1, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(1, 0, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(-1, 0, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 0, 1))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 0, -1))->toLegacy()->blockId == 0) isSafe = true;*/
				if (gm->player->region->getBlock(AirCheckPos.add(0, 1, 0))->toLegacy()->material->isReplaceable) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, -1, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(1, 0, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(-1, 0, 0))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 0, 1))->toLegacy()->blockId == 0) isSafe = true;
				if (gm->player->region->getBlock(AirCheckPos.add(0, 0, -1))->toLegacy()->blockId == 0) isSafe = true;
				if (continuemine) {
					if (name == "lit_redstone_ore" || name == "redstone_ore" && tempblockPos == blockPos2 && isSafe) {
						destroy = true;
						//selectPickaxe();
						//gm->startDestroyBlock(blockPos, 1, isDestroyed);
						tempblockPos = blockPos2; // moved
					}
				}
				else {
					if (name == "lit_redstone_ore" || name == "redstone_ore" && isSafe) {
						destroy = true;
						//selectPickaxe();
						gm->startDestroyBlock(blockPos, 1, isDestroyed2);
						continuemine = true;
						tempblockPos = blockPos2;
					}
				}

				/*if (!destroy) {
					enabledticks = 0;
				}*/


				int aids = delay - 1;
				if (destroy/* && tick >= aids*/) {
					if (swing) player->swing();
					slot = supplies->selectedHotbarSlot;
					static bool restored = false;
					if (!selectPickaxe()) {
						if (!restored) restored = true;
						else {
							auto warning = g_Data.addNotification("Regen", "No Pickaxe Found"); warning->duration = 3;
							setEnabled(false);
						}
						return;
					}
					restored = false;
					if (isregen) rot = true;
					/*if (enabledticks > 2 && tempblockPos != OldBlockPos) {
						gm->stopDestroyBlock(OldBlockPos);
						enabledticks = 0;
						clientMessageF("BlockChanged");
					} */
					//if (enabledticks > 12 && isregen) clientMessageF("IsDestroyed");
					if (firstbreak && enabledticks > 16) {
						gm->destroyBlock(&blockPos, 0); hasDestroyed = true; rot = false; enabledticks = 0; gm->stopDestroyBlock(blockPos); continuemine = false;
					}
					if (firstbreak) {
						firstbreak = false;
					}
					else firstbreak = true;
					supplies->selectedHotbarSlot = slot;
					if (!swing) player->swing();
					return;
				}

				if (hasDestroyed) { hasDestroyed = false; blocksBroke += 1; }
				if (blocksBroke > 4) blocksBroke = 0;

				//OldBlockPos = gm->player->getPos()->add(offset.toVector3());
			}

			if (!destroy) {
				//clientMessageF("Oof redstone not found");
				isregen = false;
				continuemine = false;
				canattack = true;
				enabledticks = 0;
				rot = false;
				//clientMessageF("Lost Redstone");
			}
			else
			{
				canattack = true;
			}
		}
		break;
		}
	}
}

void Regen::onPlayerTick(C_Player* plr) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;
	//if (rot && rotation) {
	//	vec2_t regenRot = player->getPos()->CalcAngle(vec3_t(tempblockPos));
	//	player->setRot(regenRot);
	//}

	if (rot && rotation) {
		vec2_t regenRot = player->getPos()->CalcAngle(vec3_t(tempblockPos));
		if (!smooth) {
			plr->yawUnused1 = regenRot.y;
			plr->pitch = regenRot.x;
			plr->bodyYaw = regenRot.y;
		}
		else {
			if (animYawB > regenRot.y) animYawB -= ((animYawB - regenRot.y) / delay);
			else if (animYawB < regenRot.y) animYawB += ((regenRot.y - animYawB) / delay);

			if (animPitchB > regenRot.x) animPitchB -= ((animPitchB - regenRot.x) / delay);
			else if (animPitchB < regenRot.x) animPitchB += ((regenRot.x - animPitchB) / delay);

			plr->yawUnused1 = animYawB;
			plr->pitch = animPitchB;
			plr->bodyYaw = animYawB;
		}
	}

	//BypassRot
	if (bypassrot && rotation) {
		vec2_t regenRot = player->getPos()->CalcAngle(vec3_t(tempblockPos));
		for (int i = 0; i < 44; i++) {
			if (animYawB > regenRot.y) animYawB -= ((animYawB - regenRot.y) / 10);
			else if (animYawB < regenRot.y) animYawB += ((regenRot.y - animYawB) / 10);

			if (animPitchB > regenRot.x) animPitchB -= ((animPitchB - regenRot.x) / 10);
			else if (animPitchB < regenRot.x) animPitchB += ((regenRot.x - animPitchB) / 10);

			plr->yawUnused1 = animYawB;
			plr->pitch = animPitchB;
			plr->bodyYaw = animYawB;
		}
	}
}

void Regen::onSendPacket(C_Packet* packet) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	if (packet->isInstanceOf<C_MovePlayerPacket>()) {
		auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
		vec2_t regenRot = player->getPos()->CalcAngle(vec3_t(tempblockPos));
		if (rot && rotation) {
			if (!smooth) {
				movePacket->pitch = regenRot.x;
				movePacket->headYaw = regenRot.y;
				movePacket->yaw = regenRot.y;
			}
			else
			{
				movePacket->pitch = animPitchB;
				movePacket->headYaw = animYawB;
				movePacket->yaw = animYawB;
			}
		}

		//BypassRot
		if (bypassrot && rotation) {
			for (int i = 0; i < 44; i++) {
				movePacket->pitch = animPitchB;
				movePacket->headYaw = animYawB;
				movePacket->yaw = animYawB;
			}
		}
	}
}

void Regen::onDisable() {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	blocksBroke = 0;
	destroy = false;
	tick = 0;
	isregen = false;
	rot = false;
	bypassrot = false;
	canattack = true;
}
