export module mini.graphics:renderer;

import mini.core;

namespace mini::graphics {

export class GRAPHICS_API Renderer {
public:
    virtual ~Renderer() = default;

    virtual void WaitForIdle() = 0;
    virtual void Render() = 0;
    virtual void Execute() = 0;

protected:
    Renderer() = default;
};

} // namespace mini::graphics