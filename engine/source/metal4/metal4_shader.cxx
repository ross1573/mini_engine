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
    ShaderLibrary(MTL::Device* device, StringView name);

    String Name() const;

    MTL::Library* GetMTLLibrary() const noexcept;

private:
    String GetFilePath(StringView name) const;
};

inline String ShaderLibrary::Name() const
{
    return m_name;
}

inline MTL::Library* ShaderLibrary::GetMTLLibrary() const noexcept
{
    return m_library.Get();
}

export class METAL4_API ShaderFunction {
private:
    SharedPtr<MTL4::FunctionDescriptor> m_descriptor;
    String m_name;

public:
    ShaderFunction(ShaderLibrary const& lib, StringView name);

    String Name() const;

    MTL4::FunctionDescriptor* GetMTLFunctionDescriptor() const noexcept;
};

inline String ShaderFunction::Name() const
{
    return m_name;
}

inline MTL4::FunctionDescriptor* ShaderFunction::GetMTLFunctionDescriptor() const noexcept
{
    return m_descriptor.Get();
}

export class METAL4_API VertexFunction final : public ShaderFunction {
public:
    VertexFunction(ShaderLibrary lib, StringView name)
        : ShaderFunction(lib, name)
    {
    }
};

export class METAL4_API FragmentFunction final : public ShaderFunction {
public:
    FragmentFunction(ShaderLibrary lib, StringView name)
        : ShaderFunction(lib, name)
    {
    }
};

} // namespace mini::metal4