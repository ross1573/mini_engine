module;

#include <regex>

export module mini.graphics:common;

import mini.core;

namespace mini::graphics {

export enum class Api : byte {
    none = 0,

    d3d12,
    metal4,
    vulkan,
};

export enum class PixelFormat : byte {
    none = 0,
    unspecialized,
    A8unorm,
    R8unorm,
    R8unormSRGB,
    R8norm,
    R8uint,
    R8int,
    R16unorm,
    R16norm,
    R16uint,
    R16int,
    R16float,
    RG8unorm,
    RG8unormSRGB,
    RG8norm,
    RG8uint,
    RG8int,
    R32uint,
    R32int,
    R32float,
    RG16unorm,
    RG16norm,
    RG16uint,
    RG16int,
    RG16float,
    RGBA8unorm,
    RGBA8unormSRGB,
    RGBA8norm,
    RGBA8uint,
    RGBA8int,
    BGRA8unorm,
    BGRA8unormSRGB,
    RG32uint,
    RG32int,
    RG32float,
    RGBA16unorm,
    RGBA16norm,
    RGBA16uint,
    RGBA16int,
    RGBA16float,
    RGBA32uint,
    RGBA32int,
    RGBA32float,
};

// NOLINTBEGIN
GRAPHICS_API const std::regex d3d12_regex("[Dd]3[Dd]12");
GRAPHICS_API const std::regex metal4_regex("[Mm][Ee][Tt][Aa][Ll]4");
GRAPHICS_API const std::regex vulkan_regex("[Vv][Uu][Ll][Kk][Aa][Nn]");
// NOLINTEND

export GRAPHICS_API Api parseApi(String const& str)
{
    char const* src = str.data();

    if (std::regex_match(src, d3d12_regex)) {
        return Api::d3d12;
    }

    if (std::regex_match(src, metal4_regex)) {
        return Api::metal4;
    }

    if (std::regex_match(src, vulkan_regex)) {
        return Api::vulkan;
    }

    return Api::none;
}

} // namespace mini::graphics

namespace fmt {

template <>
struct formatter<mini::graphics::Api> : formatter<string_view> {
    auto format(mini::graphics::Api api, format_context& ctx) const
    {
        string_view sv;
        switch (api) {
            case mini::graphics::Api::d3d12:  sv = string_view{"d3d12"}; break;
            case mini::graphics::Api::metal4: sv = string_view{"metal4"}; break;
            case mini::graphics::Api::vulkan: sv = string_view{"vulkan"}; break;

            default: sv = string_view("null"); break;
        }

        return formatter<string_view>::format(sv, ctx);
    }
};

} // namespace fmt