export module mini.metal:swap_chain;

import mini.core;
import mini.graphics;
import mini.apple;

namespace mini::metal {

export class METAL_API SwapChain final : public graphics::SwapChain {
private:
    SharedPtr<CA::MetalLayer> m_Layer;
    SharedPtr<CA::MetalDrawable> m_Drawable;

public:
    SwapChain(MTL::Device*);

    bool Initialize() final;
    void Present() final;

    void ResizeBackBuffer(uint32, uint32, bool) final;
    void SetBackBufferCount(uint8) final;
    void SetVSync(uint8) final;
    void SetFullScreen(bool) final;

    inline Vector2Int GetBackBufferSize() const final { return Vector2Int(0, 0); } // TODO
    inline uint8 GetBackBufferCount() const final { return 0; }                    // TODO
    inline uint8 GetVSync() const final { return 0; }                              // TODO
    bool GetFullScreen() const final;

    CA::MetalDrawable* GetCurrentDrawable();
    inline CA::MetalLayer* GetMetalLayer() { return m_Layer.Get(); }
};

} // namespace mini::metal