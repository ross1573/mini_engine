export module mini.windows:handle;

import mini.platform;
import mini.graphics;
import :window;

namespace mini::windows {

export class WINDOWS_API Handle final : public platform::Handle {
private:
    HINSTANCE m_Instance;

public:
    Handle();
    ~Handle() noexcept = default;

    bool IsValid() const noexcept final;

    void PollEvents() final;
    void AlertError(StringView const&) final;
    void ProcessMessage(HWND, uint32, WPARAM, LPARAM);

    HINSTANCE GetHINSTANCE() const { return m_Instance; }
};

} // namespace mini::windows