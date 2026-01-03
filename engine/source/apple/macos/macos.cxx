export module mini.macos;

export import :handle;
export import :window;

namespace mini::macos {

export class MACOS_API Interface final : public Platform {
public:
    ~Interface() noexcept final = default;

protected:
    platform::Handle* CreateHandle() final { return new macos::Handle(); }
    platform::Window* CreateWindow() final
    {
        macos::Handle* handle = static_cast<macos::Handle*>(Platform::GetHandle());
        return new macos::Window((cocoa::Application*)handle);
    }
};

} // namespace mini::macos