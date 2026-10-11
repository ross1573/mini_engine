module;

#include "cocoa_application.h"

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
    Handle() = default;
    ~Handle() noexcept = default;

    void pollEvents() final { Base::pollEvents(); }
    void alertError(StringView const& msg) final { Base::alertError(msg.data()); }

    [[nodiscard]] bool valid() const noexcept final { return Base::valid(); }

    [[nodiscard]] NSApplication* nsApplication() const noexcept { return m_application; }

protected:
    void applicationWillFinishLaunching([[maybe_unused]] NS::Notification* notification) final { }
    void applicationDidFinishLaunching([[maybe_unused]] NS::Notification* notification) final { }

    void onKeyDown(uint32 keyCode) final;
};

} // namespace mini::macos