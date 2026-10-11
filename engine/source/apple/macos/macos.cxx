module;

#include "cocoa_application.h"

export module mini.macos;

export import :handle;
export import :window;

namespace mini {

export class MACOS_API Macos final : public platform::Interface {
private:
    macos::Handle* m_handle = nullptr;
    macos::Window* m_window = nullptr;

public:
    Macos() noexcept = default;
    ~Macos() noexcept final { m_handle = nullptr; }

    [[nodiscard]] platform::Handle* createHandle() final
    {
        m_handle = new macos::Handle();
        return m_handle;
    }

    [[nodiscard]] platform::Window* createWindow() final
    {
        m_window = new macos::Window((cocoa::Application*)m_handle);
        return m_window;
    }
};

} // namespace mini