module;

#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

export module mini.metal4:swap_chain;

import mini.core;
import mini.apple;
import mini.graphics;
import :texture;

export namespace CA {

using CA::MetalDrawable;
using CA::MetalLayer;

} // namespace CA

export namespace MTL {

using MTL::Drawable;

} // namespace MTL

namespace mini::metal4 {

class Device;

} // namespace mini::metal4

namespace mini::metal4 {

class Drawable {
private:
    SharedPtr<CA::MetalDrawable> m_drawable;

public:
    Drawable() noexcept = default;

    void present() { m_drawable->present(); }
    void reset() { m_drawable.reset(); }
    void reset(CA::MetalDrawable* drawable) { m_drawable.reset(drawable); }

    [[nodiscard]] bool valid() const noexcept { return m_drawable.valid(); }
    [[nodiscard]] UniquePtr<Texture> frameTexture() const { return makeUnique<Texture>(mtlTexture()); }

    [[nodiscard]] MTL::Drawable* mtlDrawable() const noexcept { return m_drawable.get(); }
    [[nodiscard]] MTL::Texture* mtlTexture() const noexcept { return m_drawable->texture(); }
};

export class METAL4_API SwapChain final : public graphics::SwapChain {
private:
    SharedPtr<CA::MetalLayer> m_layer;
    Drawable m_drawable;

public:
    SwapChain(PtrView<Device> device);

    [[nodiscard]] bool valid() const noexcept final { return m_layer.valid(); }
    void present() final;

    void resizeFrameBuffer(uint32 width, uint32 height, bool fullscreen) final;
    void resizeBufferCount(uint8 count) final;
    void setVSync(uint8 setVSync) final;
    void setFullScreen(bool fullscreen) final;

    [[nodiscard]] Vector2Int frameBufferSize() const final { return {0, 0}; } // TODO
    [[nodiscard]] uint8 bufferCount() const final { return 0; }               // TODO
    [[nodiscard]] uint8 vsync() const final { return 0; }                     // TODO
    [[nodiscard]] bool fullScreen() const final;

    [[nodiscard]] Drawable* drawable();

    [[nodiscard]] CA::MetalLayer* caMetalLayer() const noexcept { return m_layer.get(); }
};

} // namespace mini::metal4