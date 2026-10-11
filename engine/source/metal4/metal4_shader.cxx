module;

#include <Metal/MTL4FunctionDescriptor.hpp>
#include <Metal/MTL4LibraryFunctionDescriptor.hpp>
#include <Metal/MTLLibrary.hpp>

export module mini.metal4:shader;

import mini.core;
import mini.apple;
import mini.graphics;

export namespace MTL {

using MTL::Library;

} // namespace MTL

export namespace MTL4 {

using MTL4::FunctionDescriptor;
using MTL4::LibraryFunctionDescriptor;

} // namespace MTL4

namespace mini::metal4 {

class Device;

} // namespace mini::metal4

namespace mini::metal4 {

export class METAL4_API ShaderLibrary : public graphics::ShaderLibrary {
private:
    SharedPtr<MTL::Library> m_library;

public:
    ShaderLibrary(PtrView<Device> device, StringView file);

    void setName(StringView name) { m_library->setLabel(toNSString(name).get()); }

    [[nodiscard]] bool valid() const noexcept final { return m_library.valid(); }
    [[nodiscard]] String name() const final { return toString(m_library->label()); }

    [[nodiscard]] MTL::Library* mtlLibrary() const noexcept { return m_library.get(); }

private:
    static String getFilePath(StringView name);
};

export class METAL4_API ShaderFunction : public graphics::ShaderFunction {
private:
    SharedPtr<MTL4::FunctionDescriptor> m_descriptor;
    String m_name;

public:
    ShaderFunction(PtrView<ShaderLibrary> lib, StringView name);

    [[nodiscard]] bool valid() const noexcept final { return m_descriptor.valid(); }
    [[nodiscard]] String name() const final { return m_name; }

    [[nodiscard]] MTL4::FunctionDescriptor* mtlFunctionDescriptor() const noexcept { return m_descriptor.get(); }
};

} // namespace mini::metal4