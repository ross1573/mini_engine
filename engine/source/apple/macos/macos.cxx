export module mini.macos;

export import :handle;
export import :window;

namespace mini {

export class MACOS_API Macos final : public platform::Interface {
private:
    macos::Handle* m_Handle = nullptr;

public:
    Macos() noexcept = default;
    ~Macos() noexcept final { m_Handle = nullptr; }

protected:
    platform::Handle* CreateHandle() final
    {
        m_Handle = new macos::Handle();
        return m_Handle;
    }

    platform::Window* CreateWindow() final
    {
        return new macos::Window((cocoa::Application*)m_Handle);
    }
};

} // namespace mini