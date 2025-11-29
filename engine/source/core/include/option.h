#ifndef MINI_OPTION_H
#define MINI_OPTION_H

// should be saved into a file
namespace mini::options {

inline constexpr char const* name = "Mini Engine";
inline constexpr char const* title = "Mini Engine";

inline constexpr int x = 300;
inline constexpr int y = 300;
inline constexpr int width = 1280;
inline constexpr int height = 720;

inline constexpr bool fullscreen = false;
inline constexpr bool resizableWindow = true;
inline constexpr unsigned char vsync = 0;
inline constexpr unsigned char bufferCount = 2;

#if PLATFORM_WINDOWS
inline constexpr char const* graphicsAPI = "d3d12";
#elif PLATFORM_MACOS
inline constexpr char const* graphicsAPI = "metal";
#else
inline constexpr char const* graphicsAPI = nullptr;
#endif

inline constexpr bool debugLayer = true;
inline constexpr bool gpuValidation = false;

} // namespace mini::options

#endif // MINI_OPTION_H