#ifndef MINI_OPTION_H
#define MINI_OPTION_H

// should be saved into a file
namespace mini::options {

OPTION_API extern char const* name;
OPTION_API extern char const* title;

OPTION_API extern int x;
OPTION_API extern int y;
OPTION_API extern int width;
OPTION_API extern int height;

OPTION_API extern bool fullscreen;
OPTION_API extern bool resizableWindow;
OPTION_API extern unsigned char vsync;
OPTION_API extern unsigned char bufferCount;
OPTION_API extern char const* graphicsModule;

OPTION_API extern bool debugLayer;
OPTION_API extern bool gpuValidation;

} // namespace mini::options

#endif // MINI_OPTION_H