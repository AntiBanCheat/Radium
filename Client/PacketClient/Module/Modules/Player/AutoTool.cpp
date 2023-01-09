#include "AutoTool.h"
#include "../pch.h"

AutoTool::AutoTool() : IModule(0, Category::PLAYER, "Automatically selects the best tool when mining") {
}

const char* AutoTool::getModuleName() {
	return ("AutoTool");
}

void AutoTool::onPlayerTick(C_Player* plr) {
	if (plr == nullptr) return;

	auto selectedItem = plr->getSelectedItem();

}

void AutoTool::onTick(C_GameMode* gm) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	auto selectedItem = player->getSelectedItem();

	if (g_Data.canUseMoveKeys() && !moduleMgr->getModule<ClickGUIMod>()->isEnabled()) {
		if (!g_Data.isLeftClickDown()) {
			return;
		}
		PointingStruct* pointing = player->pointingStruct;
		bool isDestroyed = false;

		C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
		C_Inventory* inv = supplies->inventory;
		auto prevSlot = supplies->selectedHotbarSlot;
		if (player->region->getBlock(pointing->block)->blockLegacy->blockId != 0) {
			for (int n = 0; n < 36; n++) {
				C_ItemStack* stack = inv->getItemStack(n);
				if (stack->item != nullptr) {
					if (stack->getItem()->isPickaxe()) {
						if (prevSlot != n) {
							supplies->selectedHotbarSlot = n;
						}
					}
				}
			}
		}
		//gm->startDestroyBlock(pointing->block, pointing->blockSide, isDestroyed);
		//supplies->selectedHotbarSlot = prevSlot;
	}
}