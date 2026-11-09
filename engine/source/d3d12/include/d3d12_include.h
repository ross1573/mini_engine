#ifndef D3D12_INCLUDE_H
#define D3D12_INCLUDE_H

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
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

#undef NOUSER
#undef NOMSG

#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>

#if DEBUG
#  include <dxgidebug.h>
#endif

#undef interface

#define COM_ARG(arg) __uuidof(**(arg)), reinterpret_cast<void**>(arg)

#define D3D12_EXPORT(type)   export using ::type;
#define D3D12_EXPORT_1(type) D3D12_EXPORT(type) D3D12_EXPORT(type##1)
#define D3D12_EXPORT_2(type) D3D12_EXPORT_1(type) D3D12_EXPORT(type##2)
#define D3D12_EXPORT_3(type) D3D12_EXPORT_2(type) D3D12_EXPORT(type##3)
#define D3D12_EXPORT_4(type) D3D12_EXPORT_3(type) D3D12_EXPORT(type##4)
#define D3D12_EXPORT_5(type) D3D12_EXPORT_4(type) D3D12_EXPORT(type##5)
#define D3D12_EXPORT_6(type) D3D12_EXPORT_5(type) D3D12_EXPORT(type##6)
#define D3D12_EXPORT_7(type) D3D12_EXPORT_6(type) D3D12_EXPORT(type##7)
#define D3D12_EXPORT_8(type) D3D12_EXPORT_7(type) D3D12_EXPORT(type##8)
#define D3D12_EXPORT_9(type) D3D12_EXPORT_8(type) D3D12_EXPORT(type##9)

#endif // D3D12_INCLUDE_H