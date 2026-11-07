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

// TODO: better way to determine mingw environment
#ifndef _MSC_VER
#  define MINGW_ABI true
#else
#  define MINGW_ABI false
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>

#if DEBUG
#  include <dxgidebug.h>
#endif

#undef interface

#define COM_ARG(type, arg) IID_##type, reinterpret_cast<void**>(arg)

#if MINGW_ABI
template <>
constexpr const GUID& __mingw_uuidof<ID3D12Debug5>()
{
    return IID_ID3D12Debug5;
}

template <>
constexpr const GUID& __mingw_uuidof<ID3D12InfoQueue>()
{
    return IID_ID3D12InfoQueue;
}
#endif

#endif // D3D12_INCLUDE_H