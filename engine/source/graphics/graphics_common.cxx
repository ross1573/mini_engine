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

namespace fmt {

template <>
struct formatter<mini::graphics::API> : formatter<string_view> {
    auto format(mini::graphics::API api, format_context& ctx) const
    {
        string_view sv;
        switch (api) {
            case mini::graphics::API::D3D12:  sv = string_view{ "d3d12" }; break;
            case mini::graphics::API::Metal:  sv = string_view{ "metal" }; break;
            case mini::graphics::API::Vulkan: sv = string_view{ "vulkan" }; break;

            case mini::graphics::API::Null: [[fallthrough]];
            default:                        sv = string_view("null"); break;
        }

        return formatter<string_view>::format(sv, ctx);
    }
};

} // namespace fmt