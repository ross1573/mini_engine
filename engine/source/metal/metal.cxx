export module mini.metal;

import mini.graphics;
export import :device;
export import :swap_chain;
export import :render_context;

namespace mini::metal {

export class METAL_API ModuleInterface : public mini::graphics::ModuleInterface {
public:
    mini::graphics::Device* CreateGraphicDevice() final { return new Device(); }
};

} // namespace mini::metal