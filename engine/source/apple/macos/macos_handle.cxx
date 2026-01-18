export module mini.macos:handle;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini::macos {

export class MACOS_API Handle final
    : protected cocoa::Application
    , public platform::Handle {
private:
    typedef cocoa::Application Base;

public:
    Handle();
    ~Handle() noexcept = default;

    bool Valid() const noexcept final { return Base::Valid(); }

    void PollEvents() final { Base::PollEvents(); }
    void AlertError(StringView const& msg) final { Base::AlertError(msg.Data()); }

    NSApplication* GetNSApplication() const noexcept;

protected:
    void ApplicationWillFinishLaunching(NS::Notification*) final {}
    void ApplicationDidFinishLaunching(NS::Notification*) final {}

    void OnKeyDown(uint32) final;
};

NSApplication* Handle::GetNSApplication() const noexcept
{
    return m_application;
}

} // namespace mini::macos