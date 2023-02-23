#include "AutoTool.h"
#include "../pch.h"

AutoTool::AutoTool() : IModule(0, Category::PLAYER, "Automatically selects the best tool when mining") {
}

const char* AutoTool::getModuleName() {
	return ("AutoTool");
}

void AutoTool::onEnable() {
	hasClicked = false;
}

void AutoTool::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr || gm->player->pointingStruct->block == vec3_t(0, 0, 0)) return;
	if (g_Data.canUseMoveKeys() && !moduleMgr->getModule<ClickGUIMod>()->isEnabled()) {
		C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
		C_Inventory* inv = supplies->inventory;
		float damage = 0;
		float speed = 0;
		if (!GameData::isLeftClickDown())
			if (hasClicked) hasClicked = false;
		if (GameData::isLeftClickDown() && !hasClicked) {
			hasClicked = true;
			for (int n = 0; n < 9; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					float currentDamage = stack->getItem()->getAttackDamage();
					bool IsUseful =
						stack->getItem()->getDestroySpeed(*stack, *g_Data.getLocalPlayer()->region->getBlock(player->pointingStruct->block)) == 5 ||
						stack->getItem()->getDestroySpeed(*stack, *g_Data.getLocalPlayer()->region->getBlock(player->pointingStruct->block)) == 4 ||
						stack->getItem()->getDestroySpeed(*stack, *g_Data.getLocalPlayer()->region->getBlock(player->pointingStruct->block)) == 3 ||
						stack->getItem()->getDestroySpeed(*stack, *g_Data.getLocalPlayer()->region->getBlock(player->pointingStruct->block)) == 2 ||
						stack->getItem()->getDestroySpeed(*stack, *g_Data.getLocalPlayer()->region->getBlock(player->pointingStruct->block)) == 1 ||
						stack->getItem()->getDestroySpeed(*stack, *g_Data.getLocalPlayer()->region->getBlock(player->pointingStruct->block)) == 0;
					if (currentDamage > damage && IsUseful) {
						damage = currentDamage;
						supplies->selectedHotbarSlot = n;
					}
				}
			}
		}
	}
}