export module mini.graphics:resource;

import mini.core;

namespace mini::graphics {

export struct GRAPHICS_API Resource {
public:
    virtual ~Resource() noexcept = default;

    [[nodiscard]] virtual uint64 GpuAddress() const = 0;
};

} // namespace mini::graphics