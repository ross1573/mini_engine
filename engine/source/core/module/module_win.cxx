module;

#include "win_include.h"

export module mini.core:module_platform;

import :string;
import :string_view;
import :memory_operation;

namespace mini {

using NativeModuleHandle = HMODULE;

NativeModuleHandle loadModule(StringView name) noexcept
{
    StringView prefix = MODULE_OUTPUT_PREFIX;
    StringView suffix = MODULE_OUTPUT_SUFFIX;

    String modulePath(prefix.size() + name.size() + suffix.size());
    modulePath.append(prefix);
    modulePath.append(name);
    modulePath.append(suffix);

    return LoadLibraryA(modulePath.data());
}

NativeModuleHandle loadMainProgram() noexcept
{
    return GetModuleHandle(nullptr);
}

void unloadModule(NativeModuleHandle handle) noexcept
{
    FreeLibrary(handle);
}

void* loadFunction(NativeModuleHandle handle, StringView name) noexcept
{
    return GetProcAddress(handle, name.data());
}

} // namespace mini