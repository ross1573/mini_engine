namespace mini::options {

OPTION_API char const* name = "Mini Engine";
OPTION_API char const* title = "Mini Engine";

OPTION_API int x = 300;
OPTION_API int y = 300;
OPTION_API int width = 1280;
OPTION_API int height = 720;

OPTION_API bool fullscreen = false;
OPTION_API bool resizableWindow = true;

#if PLATFORM_MACOS
OPTION_API unsigned char vsync = 1;
#else
OPTION_API unsigned char vsync = 0;
#endif
OPTION_API unsigned char bufferCount = 3;

#if PLATFORM_WINDOWS
OPTION_API char const* graphicsModule = "mini.d3d12";
#elif PLATFORM_MACOS
OPTION_API char const* graphicsModule = "mini.metal4";
#else
OPTION_API char const* graphicsModule = nullptr;
#endif

OPTION_API bool debugLayer = true;
OPTION_API bool gpuValidation = true;

} // namespace mini::options