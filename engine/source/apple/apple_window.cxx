module;

#include <QuartzCore/CAMetalLayer.hpp>

export module mini.apple:window;

import mini.platform;

namespace mini::apple {

export class APPLE_API Window : public platform::Window {
public:
    virtual bool IsFullScreen() const = 0;

    virtual void SetFullScreen(bool) = 0;
    virtual void SetMetalLayer(CA::MetalLayer*) = 0;
};

} // namespace mini::apple