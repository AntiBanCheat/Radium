#include "AutoSave.h"

AutoSave::AutoSave() : IModule(0, Category::OTHER, "Automatically save config") {
    registerEnumSetting("When", &when, 0);
    server.addEntry("CloseClickGUI", 0);
}

const char* AutoSave::getModuleName() {
    return "AutoSave";
}

void AutoSave::onEnable() {
}

void AutoSave::onTick(C_GameMode* gm) {
}
