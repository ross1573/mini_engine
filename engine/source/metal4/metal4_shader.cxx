export module mini.metal4:shader;

import mini.core;
import mini.apple;
import :device;

namespace mini::metal4 {

export class METAL4_API ShaderLibrary {
private:
    SharedPtr<MTL::Library> m_library;

public:
    ShaderLibrary(Device const& device, StringView name);

    void SetName(StringView name) { m_library->setLabel(ToNSString(name).Get()); }

    [[nodiscard]] bool Valid() const noexcept { return m_library.Valid(); }
    [[nodiscard]] String Name() const { return ToString(m_library->label()); }

    [[nodiscard]] MTL::Library* MTLLibrary() const noexcept { return m_library.Get(); }
    [[nodiscard]] MTL::Library* operator->() const noexcept { return m_library.operator->(); }

private:
    static String GetFilePath(StringView name);
};

export class METAL4_API ShaderFunction {
private:
    SharedPtr<MTL4::FunctionDescriptor> m_descriptor;
    String m_name;

public:
    ShaderFunction() = default;
    ShaderFunction(ShaderLibrary const& lib, StringView const& name);

    [[nodiscard]] bool Valid() const noexcept { return m_descriptor.Valid(); }
    [[nodiscard]] String Name() const { return m_name; }

    [[nodiscard]] MTL4::FunctionDescriptor* MTL4FunctionDescriptor() const noexcept { return m_descriptor.Get(); }
};

} // namespace mini::metal4