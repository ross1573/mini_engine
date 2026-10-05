#ifndef MINI_OPTION_H
#define MINI_OPTION_H

// should be saved into a file
namespace mini::options {

extern char const* name;
extern char const* title;

extern int x;
extern int y;
extern int width;
extern int height;

extern bool fullscreen;
extern bool resizableWindow;
extern unsigned char vsync;
extern unsigned char bufferCount;
extern char const* graphicsModule;

extern bool debugLayer;
extern bool gpuValidation;

} // namespace mini::options

#endif // MINI_OPTION_H