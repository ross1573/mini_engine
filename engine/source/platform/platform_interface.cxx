export module mini.platform:interface;

import mini.core;
import :handle;
import :window;

namespace mini {

export class Platform;

} // namespace mini

namespace mini::platform {

PLATFORM_API Platform* interface = nullptr;

export class PLATFORM_API Interface : public ModuleInterface {
protected:
    Platform* m_Platform;

public:
    virtual Handle* CreateHandle() = 0;
    virtual Window* CreateWindow() = 0;

protected:
    Interface() noexcept { m_Platform = interface; }
    ~Interface() noexcept { m_Platform = nullptr; }
};

} // namespace mini::platform