#ifndef WIN_INCLUDE_H
#define WIN_INCLUDE_H

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#undef NOVIRTUALKEYCODES
#undef NOWINMESSAGES
#undef NOWINSTYLES
#undef NOSHOWWINDOW
#undef NOCOLOR
#undef NOMENUS
#undef NOICONS
#undef NOSCROLL
#undef NOKEYSTATES
#undef NOMB
#undef NOUSER
#undef NOMSG

#include <unknwn.h>
#include <windows.h>

#endif // WIN_INCLUDE_H