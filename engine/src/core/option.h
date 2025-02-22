#pragma once

#include "math/rect_int.h"
#include "container/string.h"
#include "graphics/graphics.h"

namespace mini::options // should be saved into a file
{

inline constexpr char const* name = "Engine";
inline constexpr char const* title = "Engine Window";

inline constexpr RectInt windowRect = RectInt(300, 300, 1280, 720);
inline constexpr bool fullscreen = false;
inline constexpr bool resizableWindow = true;
inline constexpr uint8 vsync = 0;
inline constexpr uint8 bufferCount = 2;

inline constexpr graphics::API graphicsAPI = graphics::API::D3D12;

inline constexpr bool debugLayer = true;
inline constexpr bool gpuValidation = false;

} // namespace mini::options