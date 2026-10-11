module;

#include <dlfcn.h>

#include "cocoa_application.h"

module mini.macos;

import mini.core;
import mini.platform;
import mini.graphics;
import mini.engine;

namespace mini::macos {

void Handle::onKeyDown(uint32 keyCode)
{
    switch (keyCode) {
        case 0x0C: Engine::quit(); break;
        case 0x35: Engine::abort("escape"); break;
        default:   break;
    }
}

} // namespace mini::macos