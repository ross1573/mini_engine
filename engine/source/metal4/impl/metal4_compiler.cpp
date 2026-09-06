module mini.metal4;

import mini.core;
import mini.apple;
import :compiler;
import :common;

namespace mini::metal4 {

Compiler::Compiler(Device const& device)
{
    ASSERT(device);

    SharedPtr<MTL4::CompilerDescriptor> desc = TransferShared(MTL4::CompilerDescriptor::alloc());
    ENSURE(desc) {
        return;
    }

    desc->init();

    InitWithDescriptor(device, desc);
}

Compiler::Compiler(Device const& device, StringView name)
{
    ASSERT(device);

    SharedPtr<MTL4::CompilerDescriptor> desc = TransferShared(MTL4::CompilerDescriptor::alloc());
    ENSURE(desc) {
        return;
    }

    desc->init();
    m_name = SetLabel(desc, name);

    InitWithDescriptor(device, desc);
}

void Compiler::InitWithDescriptor(Device const& device, SharedPtr<MTL4::CompilerDescriptor> const& desc)
{
    NS::Error* error;
    m_compiler = TransferShared(device->newCompiler(desc.Get(), &error));
    ENSURE(m_compiler, error, "failed to create compiler {}", m_name.Data()) {
        return;
    }
}

} // namespace mini::metal4