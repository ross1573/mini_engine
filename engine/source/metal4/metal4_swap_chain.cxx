module;

#include <Metal/MTLDevice.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

export module mini.metal4:swap_chain;

import mini.core;
import mini.graphics;
import mini.apple;

export namespace CA {

using CA::MetalDrawable;
using CA::MetalLayer;

} // namespace CA

namespace mini::metal4 {

export class METAL4_API SwapChain final : public graphics::SwapChain {
private:
    SharedPtr<CA::MetalLayer> m_layer;
    SharedPtr<CA::MetalDrawable> m_drawable;

public:
    SwapChain(MTL::Device*);

    [[nodiscard]] bool Valid() const noexcept final { return m_layer.Valid(); }
    void Present() final;

    void ResizeBackBuffer(uint32 x, uint32 y, bool fullscreen) final;
    void SetBackBufferCount(uint8 count) final;
    void SetVSync(uint8 vsync) final;
    void SetFullScreen(bool fullscreen) final;

    [[nodiscard]] Vector2Int GetBackBufferSize() const final { return { 0, 0 }; } // TODO
    [[nodiscard]] uint8 GetBackBufferCount() const final { return 0; }            // TODO
    [[nodiscard]] uint8 GetVSync() const final { return 0; }                      // TODO
    [[nodiscard]] bool GetFullScreen() const final;

    [[nodiscard]] CA::MetalDrawable* GetCurrentDrawable();
    [[nodiscard]] CA::MetalLayer* GetMetalLayer() { return m_layer.Get(); }
};

} // namespace mini::metal4