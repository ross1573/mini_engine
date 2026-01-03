export module mini.windows;

export import mini.core;
import mini.platform;

export import :handle;
export import :window;
export import :shared_ptr;

namespace mini::windows {

class WINDOWS_API Interface final : public Platform {
public:
    ~Interface() noexcept final = default;

protected:
    platform::Handle* CreateHandle() final { return new windows::Handle(); }
    platform::Window* CreateWindow() final { return new windows::Window(); }
};

} // namespace mini::windows