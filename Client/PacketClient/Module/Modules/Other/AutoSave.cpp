#include "AutoSave.h"

using namespace std;
AutoSave::AutoSave() : IModule(0, Category::OTHER, "Automatically save config") {
  
}

const char* AutoSave::getModuleName() {
    return "AutoSave";
}

void AutoSave::onEnable() {
}

void AutoSave::onTick(C_GameMode* gm) {
}
