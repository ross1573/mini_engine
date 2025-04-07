module;

#include "quartzcore.h"

export module mini.apple:window;

import mini.platform;

namespace mini::apple {

export class APPLE_API Window : public platform::Window {
public:
    virtual void SetFullScreen(bool) = 0;
    virtual void SetMetalLayer(CAMetalLayer*) = 0;
};

} // namespace mini::apple