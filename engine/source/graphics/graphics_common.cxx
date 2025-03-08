module;

export module mini.graphics:common;

import mini.core;

export namespace mini::graphics
{

enum class API : byte
{
    Null = 0,

    D3D12,
    Metal,
    Vulkan,
};

enum class CommandType : int8
{
    None = -1,

    Direct,
    Compute,
    Copy
};

inline String ToString(API api)
{
    switch (api)
    {
        case API::D3D12: return String{"DirectX3D 12"};
        case API::Metal: return String{"Metal"};
        case API::Vulkan: return String{"Vulkan"};

        case API::Null:
        default: break;
    }

    return String{"null"};
}

} // namespace mini:::graphics