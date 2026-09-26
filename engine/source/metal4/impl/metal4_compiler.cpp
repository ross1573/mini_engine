module mini.metal4;

import mini.core;
import mini.apple;
import :compiler;
import :common;

namespace mini::metal4 {

Compiler::Compiler(Device* device)
{
    ASSERT(device);

    SharedPtr<MTL4::CompilerDescriptor> desc = TransferShared(MTL4::CompilerDescriptor::alloc());
    ENSURE(desc) {
        return;
    }

    desc->init();

    InitWithDescriptor(device, desc);
}

Compiler::Compiler(Device* device, StringView name)
{
    ASSERT(device);

    SharedPtr<NS::String> label = ToNSString(name);
    SharedPtr<MTL4::CompilerDescriptor> desc = TransferShared(MTL4::CompilerDescriptor::alloc());
    ENSURE(desc) {
        return;
    }

    desc->init();
    desc->setLabel(label.Get());

    if (label.Valid()) {
        m_name = name;
    }

    InitWithDescriptor(device, desc);
}

void Compiler::InitWithDescriptor(Device* device, SharedPtr<MTL4::CompilerDescriptor> const& desc)
{
    ASSERT(device);

    NS::Error* error;
    m_compiler = TransferShared(device->MTLDevice()->newCompiler(desc.Get(), &error));
    ENSURE(m_compiler, error, "failed to create compiler {}", m_name.Data()) {
        return;
    }
}

} // namespace mini::metal4