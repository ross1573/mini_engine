export module mini.d3d12:resource;

import mini.core;
import mini.windows;
import mini.graphics;
import :common;

D3D12_EXPORT(ID3D12Resource);
D3D12_EXPORT(D3D12_RESOURCE_DESC);

namespace mini::d3d12 {

export class D3D12_API Resource : public graphics::Resource {
protected:
    SharedPtr<ID3D12Resource> m_resource;

public:
    constexpr Resource() noexcept = default;
    explicit constexpr Resource(PtrView<ID3D12Resource> resource) noexcept;

    void setName(StringView name) { setDebugName(m_resource.get(), name); }

    [[nodiscard]] bool valid() const noexcept { return m_resource.valid(); }
    [[nodiscard]] size_t capacity() const noexcept;
    [[nodiscard]] String name() const noexcept { return debugName(m_resource.get()); }

    [[nodiscard]] ID3D12Resource* d3d12Resource() const noexcept { return m_resource.get(); }
};

constexpr Resource::Resource(PtrView<ID3D12Resource> resource) noexcept
    : m_resource(transfer(resource.get()))
{
}

} // namespace mini::d3d12