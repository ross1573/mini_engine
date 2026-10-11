module mini.metal4;

import :shader;

namespace mini::metal4 {

ShaderLibrary::ShaderLibrary(PtrView<Device> device, StringView file)
{
    String filePath = getFilePath(file);
    SharedPtr<NS::String> path = toNSString(filePath);
    NS::URL* url = NS::URL::fileURLWithPath(path.get());
    NS::Error* error;

    m_library = transfer(device->mtlDevice()->newLibrary(url, &error));
    ENSURE(m_library, error, "failed to load shader library at {}", filePath.data()) {
        return;
    }

    SharedPtr<NS::String> name = toNSString(file);
    m_library->setLabel(name.get());
}

String ShaderLibrary::getFilePath(StringView name)
{
    StringView res = NS::Bundle::mainBundle()->resourcePath()->utf8String();
    StringView ext = SHADER_LIBRARY_EXTENSION;
    String file(res.size() + name.size() + ext.size() + 1);

    file.append(res);
    file.pushBack('/');
    file.append(name);
    file.append(ext);
    return file;
}

ShaderFunction::ShaderFunction(PtrView<ShaderLibrary> lib, StringView name)
{
    SharedPtr<NS::String> nsName = toNSString(name);
    SharedPtr<MTL4::LibraryFunctionDescriptor> desc = transfer(MTL4::LibraryFunctionDescriptor::alloc());
    ENSURE(desc) {
        return;
    }

    desc->init();
    desc->setLibrary(lib->mtlLibrary());
    desc->setName(nsName.get());
    m_descriptor = staticCast<MTL4::FunctionDescriptor>(moveCast(desc));
}

} // namespace mini::metal4