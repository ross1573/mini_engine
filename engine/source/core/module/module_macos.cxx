module;

#include <dlfcn.h>

export module mini.core:module_platform;

import :string;
import :string_view;
import :memory_operation;

namespace mini {

using NativeModuleHandle = void*;

NativeModuleHandle loadModule(StringView name) noexcept
{
    StringView prefix = MODULE_OUTPUT_PREFIX;
    StringView suffix = MODULE_OUTPUT_SUFFIX;

    String modulePath(prefix.size() + name.size() + suffix.size());
    modulePath.append(prefix);
    modulePath.append(name);
    modulePath.append(suffix);

    return dlopen(modulePath.data(), RTLD_NOW | RTLD_LOCAL);
}

NativeModuleHandle loadMainProgram() noexcept
{
    return dlopen(nullptr, RTLD_NOW | RTLD_LOCAL);
}

void unloadModule(NativeModuleHandle handle) noexcept
{
    dlclose(handle);
}

void* loadFunction(NativeModuleHandle handle, StringView name) noexcept
{
    return dlsym(handle, name.data());
}

} // namespace mini