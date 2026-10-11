export module mini.graphics:shader;

import mini.core;

namespace mini::graphics {

export class GRAPHICS_API ShaderLibrary {
public:
    virtual ~ShaderLibrary() noexcept = default;

    [[nodiscard]] virtual bool valid() const noexcept = 0;
    [[nodiscard]] virtual String name() const = 0;
};

export class GRAPHICS_API ShaderFunction {
public:
    virtual ~ShaderFunction() noexcept = default;

    [[nodiscard]] virtual bool valid() const noexcept = 0;
    [[nodiscard]] virtual String name() const = 0;
};

} // namespace mini::graphics