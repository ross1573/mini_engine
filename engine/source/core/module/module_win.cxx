module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

export module mini.core:module_platform;

import :string_view;
import :memory_operation;

namespace mini {

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

void* LoadModule(StringView path)
{
    return LoadLibraryA(path.Data());
}

void UnloadModule(void* handle)
{
    FreeLibrary(handle);
}

void* LoadFunction(void* handle, StringView name)
{
    ENSURE(handle, "module not loaded") {
        return nullptr;
    }

    return GetProcAddress(handle, name.Data());
}

} // namespace mini