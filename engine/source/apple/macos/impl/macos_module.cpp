module;

#include <dlfcn.h>

module mini.macos;

import mini.core;
import :modules;

namespace mini::macos {

Module::Module(StringView name)
{
    StringView prefix = "lib" LIB_PREFIX ".";
    StringView postfix = ".dylib";

    String fullPath(prefix.Size() + name.Size() + postfix.Size());
    fullPath.Append(prefix);
    fullPath.Append(name);
    fullPath.Append(postfix);

    m_Handle = dlopen(fullPath.Data(), RTLD_NOW | RTLD_LOCAL);
}

Module::~Module()
{
    if (m_Handle != nullptr) {
        dlclose(m_Handle);
    }
}

void* Module::LoadFunc(StringView name)
{
    ENSURE(m_Handle, "module not loaded") {
        return nullptr;
    }

    void* func = dlsym(m_Handle, name.Data());
    return func;
}

} // namespace mini::macos