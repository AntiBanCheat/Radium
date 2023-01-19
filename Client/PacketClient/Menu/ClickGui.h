#pragma once

#include <map>
#include <vector>

#include "../../Memory/GameData.h"
#include "../../Utils/DrawUtils.h"
#include "../../Utils/GuiUtils.h"
#include "../Module/ModuleManager.h"
#include "../Config/ConfigManager.h"
#include "../Config/SettingManager.h"

struct ClickModule {
	bool isExtended = false;
};

struct ClickWindow {
	ClickWindow() {
		pos.x = 0;
		pos.y = 0;
		size.x = 30;
		size.y = 30;
	}
	vec2_t pos;
	vec2_t size;
	bool isExtended = true;
	bool isInAnimation = true;
	int animation = 0;
	const char* name;
	std::map<unsigned int, std::shared_ptr<ClickModule>> moduleMap;

	int yOffset = 0;
};

class ClickGui {
private:
	char categoryNameTest;
	inline static std::shared_ptr<ClickWindow> getWindow(const char* id);
	inline static std::shared_ptr<ClickModule> getClickModule(std::shared_ptr<ClickWindow> window, const char* id);

	static void renderLabel(const char* text);
	static void renderTooltip(std::string* text);

	static void renderPacketCategory(Category category, MC_Color categoryColor);
	static void renderPacketOldCategory(Category category, MC_Color categoryColor);
	static void renderVapeCategory(Category category);
	static void renderAstolfoCategory(Category category, MC_Color categoryColor, MC_Color categoryColor2);
	static void renderTenacityCategory(Category category, MC_Color categoryColor, MC_Color categoryColor2);
	static void renderTANACategory(Category category, MC_Color categoryColor);
	static void rendertenaCategory(Category category);
	inline static void getModuleListByCategory(Category category, std::vector<std::shared_ptr<IModule>>* modList);

	inline static const char* catToName(Category cat) {
		const char* categoryName;

		// Get Category Name
		{
			switch (cat) {
			case Category::COMBAT:
				categoryName = "Combat";
				break;
			case Category::VISUAL:
				categoryName = "Visual";
				break;
			case Category::MOVEMENT:
				categoryName = "Movement";
				break;
			case Category::PLAYER:
				categoryName = "Player";
				break;
			case Category::EXPLOIT:
				categoryName = "Exploit";
				break;
			case Category::OTHER:
				categoryName = "Other";
				break;
			case Category::UNUSED:
				categoryName = "";
				break;
			case Category::CUSTOM:
				categoryName = "Scripts";
				break;
			}
		}
		return categoryName;
	}

public:
	static void init();
	static void render();
	static void onKeyUpdate(int key, bool isDown);
	static void onMouseClickUpdate(int key, bool isDown);
	static void onWheelScroll(bool direction); // true = up, false = down
	static void onLoadConfig(void* confVoid);
	static void onSaveConfig(void* confVoid);
};
