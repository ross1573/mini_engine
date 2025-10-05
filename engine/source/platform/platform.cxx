export module mini.platform;

import mini.core;
export import :window;
export import :handle;
export import :module_loader;

namespace mini {

PLATFORM_API UniquePtr<platform::ModuleLoader> g_ModuleLoader;
PLATFORM_API UniquePtr<platform::Handle> g_Handle;
PLATFORM_API UniquePtr<platform::Window> g_Window;

export class PLATFORM_API Platform {
public:
    typedef mini::platform::Handle Handle;
    typedef mini::platform::Window Window;

public:
    Platform() = default;

    static bool Initialize(Handle*);
    static void Shutdown();

    static SharedPtr<Module> LoadModule(StringView);

    inline static Handle* GetHandle() noexcept { return g_Handle.Get(); }
    inline static Window* GetWindow() noexcept { return g_Window.Get(); }
};

} // namespace mini