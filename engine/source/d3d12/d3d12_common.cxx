export module mini.d3d12:common;

import mini.core;
import mini.windows;

D3D12_EXPORT(ID3D12Object);

namespace mini::d3d12 {

export enum class CommandType : int8 {
    none = -1,

    direct,
    compute,
    copy,
};

export constexpr D3D12_COMMAND_LIST_TYPE d3d12CommandType(CommandType type)
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

export constexpr CommandType commandType(D3D12_COMMAND_LIST_TYPE type)
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

export void setDebugName(PtrView<ID3D12Object> object, StringView name)
{
    object->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.data());
}

export String debugName(PtrView<ID3D12Object> object)
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