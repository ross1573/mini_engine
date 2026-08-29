module mini.metal4;

import mini.core;
import mini.apple;
import :shader;

namespace mini::metal4 {

ShaderLibrary::ShaderLibrary(MTL::Device* device, StringView name)
    : m_name(name)
{
    String filePath = GetFilePath(name);
    SharedPtr<NS::String> path = ToNSString(filePath);
    SharedPtr<NS::URL> url = TransferShared(NS::URL::fileURLWithPath(path.Get()));
    NS::Error* error;

    m_library = TransferShared(device->newLibrary(url.Get(), &error));
    ENSURE(error == nullptr, error, "failed to load shader library at {}", filePath.Data()) {
        m_library.Reset();
        error->release();
        return;
    }
}

String ShaderLibrary::GetFilePath(StringView name) const
{
    StringView path = SHADER_LIBRARY_SEARCH_PATH;
    StringView ext = SHADER_LIBRARY_EXTENSION;
    String file(path.Size() + name.Size() + ext.Size() + 2);

    file.Append(path);
    file.Push('/');
    file.Append(name);
    file.Push('.');
    file.Append(ext);
    return file;
}

ShaderFunction::ShaderFunction(ShaderLibrary const& lib, StringView name)
    : m_name(name)
{
    SharedPtr<NS::String> nsName = ToNSString(name);
    MTL4::LibraryFunctionDescriptor* libFuncDesc = MTL4::LibraryFunctionDescriptor::alloc()->init();
    libFuncDesc->setLibrary(lib.GetMTLLibrary());
    libFuncDesc->setName(nsName.Get());
}

} // namespace mini::metal4