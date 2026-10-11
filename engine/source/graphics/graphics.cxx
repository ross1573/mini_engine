export module mini.graphics;

import mini.core;

export import :common;
export import :resource;
export import :buffer;
export import :texture;
export import :shader;
export import :render_scope;
export import :render_pipeline;
export import :render_encoder;
export import :interface;
export import :device;
export import :renderer;
export import :swap_chain;

using namespace mini::graphics;

namespace mini {

export class GRAPHICS_API Graphics final : public ModuleInterface {
private:
    Api m_currentAPI;
    Module<Interface> m_currentModule;

    UniquePtr<Device> m_device;
    UniquePtr<SwapChain> m_swapChain;
    UniquePtr<Renderer> m_renderer;

public:
    Graphics() noexcept;
    ~Graphics() noexcept override;

    bool loadModule(StringView);
    void renderFrame();

    [[nodiscard]] Api api() const noexcept { return m_currentAPI; }
    [[nodiscard]] Module<Interface> module() const noexcept { return m_currentModule; }

    [[nodiscard]] Device* device() const noexcept { return m_device.get(); }
    [[nodiscard]] SwapChain* swapChain() const noexcept { return m_swapChain.get(); }
    [[nodiscard]] Renderer* renderer() const noexcept { return m_renderer.get(); }

    static void changeResolution(uint32 width, uint32 height, bool fullscreen);

    static bool current() noexcept;
    static bool current(Api api) noexcept;
};

} // namespace mini