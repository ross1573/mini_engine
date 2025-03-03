module;

export module mini.graphics:swap_chain;

import mini.core;

namespace mini { class Graphics; }

export namespace mini::graphics
{

class GRAPHICS_API SwapChain
{
    friend class mini::Graphics;

protected:
    static constexpr uint8 MaxBackBuffer = 3;

public:
    virtual ~SwapChain() = default;

    virtual void ResizeBackBuffer(uint32, uint32, bool) = 0;
    virtual void SetBackBufferCount(uint8) = 0;
    virtual void SetVSync(uint8) = 0;
    virtual void SetFullScreen(bool) = 0;

    virtual Vector2Int GetBackBufferSize() const = 0;
    virtual uint8 GetBackBufferCount() const = 0;
    virtual uint8 GetVSync() const = 0;
    virtual bool GetFullscreen() const = 0;

protected:
    SwapChain() = default;

    virtual bool Initialize() = 0;
    virtual void Present() = 0;
};

} // namespace mini::graphics