module;

#include <Metal/MTL4Compiler.hpp>

export module mini.metal4:compiler;

import mini.core;
import mini.apple;

export namespace MTL4 {

using MTL4::Compiler;
using MTL4::CompilerDescriptor;
using MTL4::CompilerTaskOptions;

} // namespace MTL4

namespace mini::metal4 {

class Device;

} // namespace mini::metal4

namespace mini::metal4 {

export class METAL4_API Compiler {
private:
    SharedPtr<MTL4::Compiler> m_compiler;
    String m_name;

public:
    explicit Compiler(PtrView<Device> device, StringView name = StringView{});

    [[nodiscard]] bool valid() const noexcept { return m_compiler.valid(); }
    [[nodiscard]] String name() const { return m_name; }

    [[nodiscard]] MTL4::Compiler* mtlCompiler() const noexcept { return m_compiler.get(); }
};

} // namespace mini::metal4