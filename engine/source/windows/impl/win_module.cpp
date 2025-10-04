module mini.windows;

import mini.core;
import :modules;

namespace mini::windows {

Module::Module(StringView name)
    : m_Name(name)
{
    StringView prefix = LIB_PREFIX ".";
    StringView postfix = ".dll";

    String modulePath(prefix.Size() + name.Size() + postfix.Size());
    modulePath.Append(prefix);
    modulePath.Append(name);
    modulePath.Append(postfix);

    m_Handle = LoadLibraryA(modulePath.Data());
}

Module::~Module()
{
    if (m_Handle != nullptr) {
        FreeLibrary(m_Handle);
    }
}

void* Module::LoadFunc(StringView name)
{
    ENSURE(m_Handle, "module not loaded") {
        return nullptr;
    }

    void* func = GetProcAddress(m_Handle, name.Data());
    return func;
}

} // namespace mini::windows