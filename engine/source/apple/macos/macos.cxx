export module mini.macos;

export import :handle;
export import :window;

namespace mini {

export class MACOS_API Macos final : public platform::Interface {
private:
    macos::Handle* m_handle = nullptr;

public:
    Macos() noexcept = default;
    ~Macos() noexcept { m_handle = nullptr; }

protected:
    platform::Handle* CreateHandle() final
    {
        m_handle = new macos::Handle();
        return m_handle;
    }

    platform::Window* CreateWindow() final
    {
        return new macos::Window((cocoa::Application*)m_handle);
    }
};

} // namespace mini