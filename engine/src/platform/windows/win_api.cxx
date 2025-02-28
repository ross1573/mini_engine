module;

#include "core/define.h"

export module mini.windows:api;

export import <windows.h>;

#ifdef DEBUG
#   define _CRTDBG_MAP_ALLOC
export import <crtdbg.h>;
#endif