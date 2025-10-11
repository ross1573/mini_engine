export module mini.metal;

export import :device;
export import :swap_chain;
export import :render_context;

import mini.graphics;

namespace mini::metal {

export class METAL_API ModuleInterface : public mini::graphics::ModuleInterface {
public:
    mini::graphics::Device* CreateGraphicDevice() final { return new Device(); }
};

} // namespace mini::metal