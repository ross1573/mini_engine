module;

#include <Metal/MTL4Compiler.hpp>

export module mini.metal4:compiler;

import mini.core;
import mini.apple;
import :device;

export namespace MTL4 {

using MTL4::Compiler;
using MTL4::CompilerDescriptor;
using MTL4::CompilerTaskOptions;

} // namespace MTL4

namespace mini::metal4 {

export class METAL4_API Compiler {
private:
    SharedPtr<MTL4::Compiler> m_compiler;
    String m_name;

public:
    explicit Compiler(Device* device);
    Compiler(Device* device, StringView name);

    [[nodiscard]] bool Valid() const noexcept { return m_compiler.Valid(); }
    [[nodiscard]] String Name() const { return m_name; }

    [[nodiscard]] MTL4::Compiler* MTLCompiler() const noexcept { return m_compiler.Get(); }

private:
    void InitWithDescriptor(Device* device, SharedPtr<MTL4::CompilerDescriptor> const& desc);
};

} // namespace mini::metal4