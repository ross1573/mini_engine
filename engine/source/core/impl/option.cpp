namespace mini::options {

CORE_API char const* name = "Mini Engine";
CORE_API char const* title = "Mini Engine";

CORE_API int x = 300;
CORE_API int y = 300;
CORE_API int width = 1280;
CORE_API int height = 720;

CORE_API bool fullscreen = false;
CORE_API bool resizableWindow = true;

#if PLATFORM_MACOS
CORE_API unsigned char vsync = 1;
#else
CORE_API unsigned char vsync = 0;
#endif
CORE_API unsigned char bufferCount = 3;

#if PLATFORM_WINDOWS
CORE_API char const* graphicsModule = "mini.d3d12";
#elif PLATFORM_MACOS
CORE_API char const* graphicsModule = "mini.metal4";
#else
CORE_API char const* graphicsModule = nullptr;
#endif

CORE_API bool debugLayer = true;
CORE_API bool gpuValidation = true;

} // namespace mini::options