export module mini.options;

import mini.core;
import mini.graphics;

export namespace mini::options // should be saved into a file
{

inline constexpr char const* name = "Engine";
inline constexpr char const* title = "Engine Window";

inline constexpr RectInt windowRect = RectInt(300, 300, 1280, 720);
inline constexpr uint32 x = 300;
inline constexpr uint32 y = 300;
inline constexpr uint32 width = 1280;
inline constexpr uint32 height = 720; 

inline constexpr bool fullscreen = false;
inline constexpr bool resizableWindow = true;
inline constexpr uint8 vsync = 0;
inline constexpr uint8 bufferCount = 2;

inline constexpr graphics::API graphicsAPI = graphics::API::D3D12;

inline constexpr bool debugLayer = true;
inline constexpr bool gpuValidation = false;

} // namespace mini::options