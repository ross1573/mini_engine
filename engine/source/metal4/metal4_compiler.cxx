export module mini.metal4:compiler;

import mini.core;
import mini.apple;
import :device;

namespace mini::metal4 {

export class METAL4_API Compiler {
private:
    SharedPtr<MTL4::Compiler> m_compiler;
    String m_name;

public:
    explicit Compiler(Device const& device);
    Compiler(Device const& device, StringView name);

    bool Valid() const noexcept { return m_compiler.Valid(); }
    String Name() const { return m_name; }

    MTL4::Compiler* MTL4Compiler() const noexcept { return m_compiler.Get(); }
    MTL4::Compiler* operator->() const noexcept { return m_compiler.operator->(); }

private:
    void InitWithDescriptor(Device const& device, SharedPtr<MTL4::CompilerDescriptor> const& desc);
};

} // namespace mini::metal4