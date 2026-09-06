module mini.metal4;

import mini.core;
import mini.apple;
import :shader;

namespace mini::metal4 {

ShaderLibrary::ShaderLibrary(Device const& device, StringView name)
{
    String filePath = GetFilePath(name);
    SharedPtr<NS::String> path = ToNSString(filePath);
    NS::URL* url = NS::URL::fileURLWithPath(path.Get());
    NS::Error* error;

    m_library = TransferShared(device->newLibrary(url, &error));
    ENSURE(m_library, error, "failed to load shader library at {}", filePath.Data()) {
        return;
    }

    SetName(name);
}

void ShaderLibrary::SetName(StringView name)
{
    if (m_name == name) {
        return;
    }

    m_name = SetLabel(m_library, name);
}

String ShaderLibrary::GetFilePath(StringView name) const
{
    StringView res = NS::Bundle::mainBundle()->resourcePath()->utf8String();
    StringView ext = SHADER_LIBRARY_EXTENSION;
    String file(res.Size() + name.Size() + ext.Size() + 1);

    file.Append(res);
    file.Push('/');
    file.Append(name);
    file.Append(ext);
    return file;
}

ShaderFunction::ShaderFunction(ShaderLibrary const& lib, StringView const& name)
{
    SharedPtr<NS::String> nsName = ToNSString(name);
    SharedPtr<MTL4::LibraryFunctionDescriptor> desc = TransferShared(MTL4::LibraryFunctionDescriptor::alloc());
    ENSURE(desc) {
        return;
    }

    desc->init();
    desc->setLibrary(lib.MTLLibrary());
    desc->setName(nsName.Get());

    m_descriptor = StaticCast<MTL4::FunctionDescriptor>(MoveArg(desc));
}

} // namespace mini::metal4