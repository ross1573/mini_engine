export module mini.platform;

import mini.core;

export import :interface;
export import :window;
export import :handle;

using namespace mini::platform;

namespace mini {

export class PLATFORM_API Platform final : public ModuleInterface {
private:
    Module<Interface> m_nativeModule;
    UniquePtr<Handle> m_handle;
    UniquePtr<Window> m_window;

public:
    Platform() noexcept;
    ~Platform() noexcept;

    void pollEvents();

    [[nodiscard]] Handle* handle() const noexcept { return m_handle.get(); }
    [[nodiscard]] Window* window() const noexcept { return m_window.get(); }

    static void alertError(StringView const&);
};

} // namespace mini