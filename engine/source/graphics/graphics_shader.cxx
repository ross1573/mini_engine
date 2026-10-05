export module mini.graphics:shader;

import mini.core;

namespace mini::graphics {

export class GRAPHICS_API ShaderLibrary {
public:
    virtual ~ShaderLibrary() noexcept = default;

    [[nodiscard]] virtual bool Valid() const noexcept = 0;
    [[nodiscard]] virtual String Name() const = 0;
};

export class GRAPHICS_API ShaderFunction {
public:
    virtual ~ShaderFunction() noexcept = default;

    [[nodiscard]] virtual bool Valid() const noexcept = 0;
    [[nodiscard]] virtual String Name() const = 0;
};

} // namespace mini::graphics