module;

#include <regex>

export module mini.graphics:common;

import mini.core;

namespace mini::graphics {

export enum class API : byte {
    Null = 0,

    D3D12,
    Metal,
    Vulkan,
};

export enum class CommandType : int8 {
    None = -1,

    Direct,
    Compute,
    Copy
};

export GRAPHICS_API inline constexpr StringView format_as(API api)
{
    switch (api) {
        case API::D3D12:  return StringView{ "d3d12" };
        case API::Metal:  return StringView{ "metal" };
        case API::Vulkan: return StringView{ "vulkan" };

        case API::Null: [[fallthrough]];
        default:        break;
    }

    return StringView{ "null" };
}

GRAPHICS_API const std::regex d3d12_regex("[Dd]3[Dd]12");
GRAPHICS_API const std::regex metal_regex("[Mm][Ee][Tt][Aa][Ll]");
GRAPHICS_API const std::regex vulkan_regex("[Vv][Uu][Ll][Kk][Aa][Nn]");

export GRAPHICS_API API ParseAPI(String const& str)
{
    char const* src = str.Data();
    if (std::regex_match(src, d3d12_regex)) {
        return API::D3D12;
    }
    else if (std::regex_match(src, metal_regex)) {
        return API::Metal;
    }
    else if (std::regex_match(src, vulkan_regex)) {
        return API::Vulkan;
    }
    return API::Null;
}

} // namespace mini::graphics