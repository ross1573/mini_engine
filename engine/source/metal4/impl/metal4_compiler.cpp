module mini.metal4;

import :compiler;
import :common;

namespace mini::metal4 {

Compiler::Compiler(PtrView<Device> device, StringView name)
{
    ASSERT(device);

    SharedPtr<MTL4::CompilerDescriptor> desc = transfer(MTL4::CompilerDescriptor::alloc()->init());

    if (!name.empty()) {
        SharedPtr<NS::String> label = toNSString(name);
        if (label.valid()) {
            desc->setLabel(label.get());
            m_name = name;
        }
    }

    NS::Error* error;
    MTL4::Compiler* compiler = device->mtlDevice()->newCompiler(desc.get(), &error);
    ENSURE(compiler, error, "failed to create compiler {}", m_name.data()) {
        return;
    }

    m_compiler = transfer(compiler);
}

} // namespace mini::metal4