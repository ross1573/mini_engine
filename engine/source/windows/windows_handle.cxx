export module mini.windows:handle;

import mini.platform;
import mini.graphics;
import :window;

namespace mini::windows {

export class WINDOWS_API Handle final : public platform::Handle {
private:
    HINSTANCE m_instance;

public:
    Handle();
    ~Handle() noexcept = default;

    bool valid() const noexcept final;

    void pollEvents() final;
    void alertError(StringView const&) final;
    void ProcessMessage(HWND, uint32, WPARAM, LPARAM);

    HINSTANCE GetHINSTANCE() const { return m_instance; }
};

} // namespace mini::windows