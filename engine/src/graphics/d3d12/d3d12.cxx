module;

#include "core/define.h"
#pragma warning(push)
#pragma warning(disable: 5202) // msvc complains about doing weird things

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "D3DCompiler.lib")

#if defined(_M_ARM64EC)
    DECL_DLL_SPEC(extern const char* D3D12SDKPath = "./engine/external/D3D12/arm64/")
#elif defined(_M_ARM64)
    DECL_DLL_SPEC(extern const char* D3D12SDKPath = "./engine/external/D3D12/arm64/")
#elif defined(_M_AMD64)
    DECL_DLL_SPEC(extern const char* D3D12SDKPath = "./engine/external/D3D12/x64/")
#elif defined(_M_IX86)
    DECL_DLL_SPEC(extern const char* D3D12SDKPath = "./engine/external/D3D12/win32/")
#endif

#pragma warning(pop)

export module mini.d3d12;

export import :device;
export import :render_context;
export import :swap_chain;