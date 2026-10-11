module;

#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

export module mini.apple:window;

import mini.platform;

export namespace CA {

using CA::MetalDrawable;
using CA::MetalLayer;

} // namespace CA

namespace mini::apple {

export class APPLE_API Window : public platform::Window {
public:
    virtual bool fullScreen() const = 0;

    virtual void setFullScreen(bool) = 0;
    virtual void setMetalLayer(CA::MetalLayer*) = 0;
};

} // namespace mini::apple