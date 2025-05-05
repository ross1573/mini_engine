module;

#include "assertion.h"
#include "option.h"
#include "resource.h"

module mini.windows;

import mini.core;
import mini.engine;
import mini.graphics;
import mini.platform;

namespace mini::windows {

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Handle::Handle(HINSTANCE inst)
    : m_Instance(inst)
{
    ASSERT(m_Instance);
}

Handle::~Handle()
{
}

bool Handle::Initialize()
{
    auto className = mini::options::name;

    WNDCLASSEX wcex = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = m_Instance,
        .hIcon = LoadIconA(m_Instance, MAKEINTRESOURCE(IDI_DIRECTX12)),
        .hCursor = LoadCursorA(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = MAKEINTRESOURCE(IDC_DIRECTX12),
        .lpszClassName = className,
        .hIconSm = LoadIconA(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)),
    };

    RegisterClassExA(&wcex);
    return true;
}

platform::Window* Handle::CreatePlatformWindow()
{
    return new windows::Window();
}

graphics::Device* Handle::CreateGraphicDevice(graphics::API api)
{
    HMODULE graphicModule = nullptr;
    FARPROC createDeviceAddr = nullptr;

    switch (api) {
        case graphics::API::D3D12:
            graphicModule = LoadLibraryA(LIB_PREFIX ".d3d12.dll");
            createDeviceAddr = GetProcAddress(graphicModule, "CreateGraphicDevice");
            break;

        default: break;
    }

    ENSURE(createDeviceAddr, "unable to find graphics module init function") return nullptr;

    typedef graphics::Device* (*CreateDeviceFuncT)();
    CreateDeviceFuncT createDeviceFunc = reinterpret_cast<CreateDeviceFuncT>(createDeviceAddr);
    return createDeviceFunc();
}

void Handle::PollEvents()
{
    MSG msg{};

    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        if (msg.message == WM_QUIT) {
            Engine::Quit();
        }
    }
}

void Handle::ProcessMessage([[maybe_unused]] HWND hWnd, [[maybe_unused]] uint32 msg,
                            [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (msg) {
        case WM_DESTROY: PostQuitMessage(0); break;

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE: Engine::Abort("escape"); break;
                case 'Q':       Engine::Quit(); break;
                default:        break;
            }
            break;

        case WM_SIZE: Graphics::ChangeResolution(LOWORD(lParam), HIWORD(lParam), false); break;

        default: return;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Handle* handle = (Handle*)Platform::GetHandle();
    handle->ProcessMessage(hWnd, message, wParam, lParam);
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

} // namespace mini::windows