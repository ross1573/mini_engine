export module mini.metal4:shader;

import mini.core;
import mini.apple;
import :device;

namespace mini::metal4 {

export class METAL4_API ShaderLibrary {
private:
    SharedPtr<MTL::Library> m_library;
    String m_name;

public:
    ShaderLibrary(Device const& device, StringView name);

    void SetName(StringView name);

    bool Valid() const noexcept { return m_library.Valid(); }
    String Name() const { return m_name; }

    MTL::Library* MTLLibrary() const noexcept { return m_library.Get(); }
    MTL::Library* operator->() const noexcept { return m_library.operator->(); }

private:
    String GetFilePath(StringView name) const;
};

export class METAL4_API ShaderFunction {
private:
    SharedPtr<MTL4::FunctionDescriptor> m_descriptor;
    String m_name;

public:
    ShaderFunction() = default;
    ShaderFunction(ShaderLibrary const& lib, StringView const& name);

    bool Valid() const noexcept { return m_descriptor.Valid(); }
    String Name() const { return m_name; }

    MTL4::FunctionDescriptor* MTL4FunctionDescriptor() const noexcept { return m_descriptor.Get(); }
};

} // namespace mini::metal4