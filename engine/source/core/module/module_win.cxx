module;

#include "internal/min_windows.h"

export module mini.core:module_platform;

import :string;
import :string_view;
import :memory_operation;

namespace mini {

using NativeModuleHandle = HMODULE;

String BuildModulePath(StringView name)
{
    StringView prefix = MODULE_OUTPUT_PREFIX ".";
    StringView suffix = MODULE_OUTPUT_SUFFIX;

    String modulePath(prefix.Size() + name.Size() + suffix.Size());
    modulePath.Append(prefix);
    modulePath.Append(name);
    modulePath.Append(suffix);

    return modulePath;
}

NativeModuleHandle LoadModule(StringView path)
{
    return LoadLibraryA(path.Data());
}

void UnloadModule(NativeModuleHandle handle)
{
    FreeLibrary(handle);
}

void* LoadFunction(NativeModuleHandle handle, StringView name)
{
    ENSURE(handle, "module not loaded") {
        return nullptr;
    }

    return GetProcAddress(handle, name.Data());
}

} // namespace mini