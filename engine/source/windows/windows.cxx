export module mini.windows;

import mini.platform;

export import :assert;
export import :handle;
export import :window;
export import :shared_ptr;

namespace mini {

export class WINDOWS_API Windows final : public platform::Interface {
public:
    Windows() noexcept;
    ~Windows() noexcept final;

    windows::Handle* handle() noexcept;
    windows::Window* window() noexcept;

protected:
    platform::Handle* createHandle() final;
    platform::Window* createWindow() final;
};

} // namespace mini

namespace mini::windows {

WINDOWS_API Windows* interface = nullptr;

} // namespace mini::windows