export module mini.graphics:device;

import :common;
import :renderer;
import :swap_chain;

namespace mini::graphics {

export class GRAPHICS_API Device {
public:
    virtual ~Device() = default;

    [[nodiscard]] virtual SwapChain* createSwapChain() = 0;
    [[nodiscard]] virtual Renderer* createRenderer() = 0;

    [[nodiscard]] virtual bool valid() const noexcept = 0;
    [[nodiscard]] virtual Api api() const = 0;

protected:
    Device() = default;
};

} // namespace mini::graphics