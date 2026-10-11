module;

export module mini.graphics:swap_chain;

import mini.core;

namespace mini::graphics {

export class GRAPHICS_API SwapChain {
protected:
    static constexpr uint8 maxFrameBuffer = 3;

public:
    virtual ~SwapChain() = default;

    virtual void present() = 0;

    virtual void resizeFrameBuffer(uint32, uint32, bool) = 0;
    virtual void resizeBufferCount(uint8) = 0;
    virtual void setVSync(uint8) = 0;
    virtual void setFullScreen(bool) = 0;

    [[nodiscard]] virtual bool valid() const noexcept = 0;
    [[nodiscard]] virtual bool fullScreen() const = 0;
    [[nodiscard]] virtual Vector2Int frameBufferSize() const = 0;
    [[nodiscard]] virtual uint8 bufferCount() const = 0;
    [[nodiscard]] virtual uint8 vsync() const = 0;

protected:
    SwapChain() noexcept = default;
};

} // namespace mini::graphics