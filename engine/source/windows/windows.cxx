export module mini.windows;

export import mini.core;
import mini.platform;

export import :handle;
export import :window;
export import :shared_ptr;

namespace mini {

export class WINDOWS_API Windows final : public platform::Interface {
public:
    Windows() noexcept;
    ~Windows() noexcept;

    windows::Handle* GetHandle() noexcept;
    windows::Window* GetWindow() noexcept;

protected:
    platform::Handle* CreateHandle() final;
    platform::Window* CreateWindow() final;
};

} // namespace mini

namespace mini::windows {

WINDOWS_API Windows* interface = nullptr;

} // namespace mini::windows