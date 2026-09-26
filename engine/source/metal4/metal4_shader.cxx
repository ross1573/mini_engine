module;

#include <Metal/MTL4FunctionDescriptor.hpp>
#include <Metal/MTL4LibraryFunctionDescriptor.hpp>
#include <Metal/MTLLibrary.hpp>

export module mini.metal4:shader;

import mini.core;
import mini.apple;
import :device;

export namespace MTL {

using MTL::Library;

} // namespace MTL

export namespace MTL4 {

using MTL4::FunctionDescriptor;
using MTL4::LibraryFunctionDescriptor;

} // namespace MTL4

namespace mini::metal4 {

export class METAL4_API ShaderLibrary {
private:
    SharedPtr<MTL::Library> m_library;

public:
    ShaderLibrary(Device* device, StringView file);

    void SetName(StringView name) { m_library->setLabel(ToNSString(name).Get()); }

    [[nodiscard]] bool Valid() const noexcept { return m_library.Valid(); }
    [[nodiscard]] String Name() const { return ToString(m_library->label()); }

    [[nodiscard]] MTL::Library* MTLLibrary() const noexcept { return m_library.Get(); }

private:
    static String GetFilePath(StringView name);
};

export class METAL4_API ShaderFunction {
private:
    SharedPtr<MTL4::FunctionDescriptor> m_descriptor;
    String m_name;

public:
    ShaderFunction(ShaderLibrary* lib, StringView name);

    [[nodiscard]] bool Valid() const noexcept { return m_descriptor.Valid(); }
    [[nodiscard]] String Name() const { return m_name; }

    [[nodiscard]] MTL4::FunctionDescriptor* MTLFunctionDescriptor() const noexcept { return m_descriptor.Get(); }
};

} // namespace mini::metal4