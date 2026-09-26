module;

#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

export module mini.metal4:swap_chain;

import mini.core;
import mini.graphics;
import mini.apple;
import :device;
import :texture;

export namespace CA {

using CA::MetalDrawable;
using CA::MetalLayer;

} // namespace CA

export namespace MTL {

using MTL::Drawable;

} // namespace MTL

namespace mini::metal4 {

class Drawable {
private:
    SharedPtr<CA::MetalDrawable> m_drawable;

public:
    Drawable() noexcept = default;

    void Present() { m_drawable->present(); }
    void Reset() { m_drawable.Reset(); }
    void Reset(CA::MetalDrawable* drawable) { m_drawable.Reset(drawable); }

    [[nodiscard]] bool Valid() const noexcept { return m_drawable.Valid(); }

    [[nodiscard]] MTL::Drawable* MTLDrawable() const noexcept { return m_drawable.Get(); }
    [[nodiscard]] MTL::Texture* MTLTexture() const noexcept { return m_drawable->texture(); }
};

export class METAL4_API SwapChain final : public graphics::SwapChain {
private:
    SharedPtr<CA::MetalLayer> m_layer;
    Drawable m_drawable;

public:
    SwapChain(Device*);

    [[nodiscard]] bool Valid() const noexcept final { return m_layer.Valid(); }
    void Present() final;

    void ResizeBackBuffer(uint32 width, uint32 height, bool fullscreen) final;
    void SetBackBufferCount(uint8 count) final;
    void SetVSync(uint8 vsync) final;
    void SetFullScreen(bool fullscreen) final;

    [[nodiscard]] Vector2Int GetBackBufferSize() const final { return {0, 0}; } // TODO
    [[nodiscard]] uint8 GetBackBufferCount() const final { return 0; }          // TODO
    [[nodiscard]] uint8 GetVSync() const final { return 0; }                    // TODO
    [[nodiscard]] bool GetFullScreen() const final;

    [[nodiscard]] UniquePtr<Texture> FrameTexture();
    [[nodiscard]] Drawable* Drawable();

private:
    void SwapNextDrawable();
};

} // namespace mini::metal4