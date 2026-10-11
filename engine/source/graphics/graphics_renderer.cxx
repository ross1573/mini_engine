export module mini.graphics:renderer;

import mini.core;

namespace mini::graphics {

export class GRAPHICS_API Renderer {
public:
    virtual ~Renderer() = default;

    virtual void prepare() = 0;
    virtual void render() = 0;
    virtual void waitIdle() = 0;

protected:
    Renderer() noexcept = default;
};

} // namespace mini::graphics