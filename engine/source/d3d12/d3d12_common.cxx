export module mini.d3d12:common;

import mini.core;
import mini.windows;
import mini.graphics;

D3D12_EXPORT(ID3D12Object);
D3D12_EXPORT(DXGI_FORMAT);

namespace mini::d3d12 {

export enum class CommandType : int8 {
    none = -1,

    direct,
    compute,
    copy,
};

export D3D12_API constexpr DXGI_FORMAT dxgiFormat(graphics::PixelFormat pixelFormat) noexcept
{
    switch (pixelFormat) {
        case graphics::PixelFormat::none:           return DXGI_FORMAT_UNKNOWN;
        case graphics::PixelFormat::unspecialized:  return DXGI_FORMAT_UNKNOWN;
        case graphics::PixelFormat::A8unorm:        return DXGI_FORMAT_A8_UNORM;
        case graphics::PixelFormat::R8unorm:        return DXGI_FORMAT_R8_UNORM;
        case graphics::PixelFormat::R8norm:         return DXGI_FORMAT_R8_SNORM;
        case graphics::PixelFormat::R8uint:         return DXGI_FORMAT_R8_UINT;
        case graphics::PixelFormat::R8int:          return DXGI_FORMAT_R8_SINT;
        case graphics::PixelFormat::R16unorm:       return DXGI_FORMAT_R16_UNORM;
        case graphics::PixelFormat::R16norm:        return DXGI_FORMAT_R16_SNORM;
        case graphics::PixelFormat::R16uint:        return DXGI_FORMAT_R16_UINT;
        case graphics::PixelFormat::R16int:         return DXGI_FORMAT_R16_SINT;
        case graphics::PixelFormat::R16float:       return DXGI_FORMAT_R16_FLOAT;
        case graphics::PixelFormat::RG8unorm:       return DXGI_FORMAT_R8G8_UNORM;
        case graphics::PixelFormat::RG8norm:        return DXGI_FORMAT_R8G8_SNORM;
        case graphics::PixelFormat::RG8uint:        return DXGI_FORMAT_R8G8_UINT;
        case graphics::PixelFormat::RG8int:         return DXGI_FORMAT_R8G8_SINT;
        case graphics::PixelFormat::R32uint:        return DXGI_FORMAT_R32_UINT;
        case graphics::PixelFormat::R32int:         return DXGI_FORMAT_R32_SINT;
        case graphics::PixelFormat::R32float:       return DXGI_FORMAT_R32_FLOAT;
        case graphics::PixelFormat::RG16unorm:      return DXGI_FORMAT_R16G16_UNORM;
        case graphics::PixelFormat::RG16norm:       return DXGI_FORMAT_R16G16_SNORM;
        case graphics::PixelFormat::RG16uint:       return DXGI_FORMAT_R16G16_UINT;
        case graphics::PixelFormat::RG16int:        return DXGI_FORMAT_R16G16_SINT;
        case graphics::PixelFormat::RG16float:      return DXGI_FORMAT_R16G16_FLOAT;
        case graphics::PixelFormat::RGBA8unorm:     return DXGI_FORMAT_R8G8B8A8_UNORM;
        case graphics::PixelFormat::RGBA8unormSRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case graphics::PixelFormat::RGBA8norm:      return DXGI_FORMAT_R8G8B8A8_SNORM;
        case graphics::PixelFormat::RGBA8uint:      return DXGI_FORMAT_R8G8B8A8_UINT;
        case graphics::PixelFormat::RGBA8int:       return DXGI_FORMAT_R8G8B8A8_SINT;
        case graphics::PixelFormat::BGRA8unorm:     return DXGI_FORMAT_B8G8R8A8_UNORM;
        case graphics::PixelFormat::BGRA8unormSRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case graphics::PixelFormat::RG32uint:       return DXGI_FORMAT_R32G32_UINT;
        case graphics::PixelFormat::RG32int:        return DXGI_FORMAT_R32G32_SINT;
        case graphics::PixelFormat::RG32float:      return DXGI_FORMAT_R32G32_FLOAT;
        case graphics::PixelFormat::RGBA16unorm:    return DXGI_FORMAT_R16G16B16A16_UNORM;
        case graphics::PixelFormat::RGBA16norm:     return DXGI_FORMAT_R16G16B16A16_SNORM;
        case graphics::PixelFormat::RGBA16uint:     return DXGI_FORMAT_R16G16B16A16_UINT;
        case graphics::PixelFormat::RGBA16int:      return DXGI_FORMAT_R16G16B16A16_SINT;
        case graphics::PixelFormat::RGBA16float:    return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case graphics::PixelFormat::RGBA32uint:     return DXGI_FORMAT_R32G32B32A32_UINT;
        case graphics::PixelFormat::RGBA32int:      return DXGI_FORMAT_R32G32B32A32_SINT;
        case graphics::PixelFormat::RGBA32float:    return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    ASSERT(pixelFormat == graphics::PixelFormat::none, "invalid pixel format {}.", static_cast<byte>(pixelFormat));
    return DXGI_FORMAT_UNKNOWN;
}

export D3D12_API constexpr D3D12_COMMAND_LIST_TYPE d3d12CommandType(CommandType type)
{
    switch (type) {
        case CommandType::direct:  return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandType::copy:    return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandType::compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        default:                   break;
    }

    ASSERT(type != CommandType::direct, "invalid CommandType {}", static_cast<int32>(type));
    return D3D12_COMMAND_LIST_TYPE_DIRECT;
}

export D3D12_API constexpr CommandType commandType(D3D12_COMMAND_LIST_TYPE type)
{
    switch (type) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:  return CommandType::direct;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: return CommandType::compute;
        case D3D12_COMMAND_LIST_TYPE_COPY:    return CommandType::copy;
        default:                              break;
    }

    ASSERT(type != D3D12_COMMAND_LIST_TYPE_DIRECT, "unsupported D3D12_COMMAND_LIST_TYPE {}", static_cast<int32>(type));
    return CommandType::direct;
}

export D3D12_API void setDebugName(PtrView<ID3D12Object> object, StringView name)
{
    object->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.data());
}

export D3D12_API String debugName(PtrView<ID3D12Object> object)
{
    UINT size = 0;
    HRESULT result = object->GetPrivateData(WKPDID_D3DDebugObjectName, &size, nullptr);
    if (FAILED(result) || size == 0) {
        return String{};
    }

    String name;
    name.resize(static_cast<size_t>(size));
    ENSURE(object->GetPrivateData(WKPDID_D3DDebugObjectName, &size, name.data())) {
        return String{};
    }

    return name;
}

} // namespace mini::d3d12