module;

#include <windows.h>
#include "resource/Resource.h"
#include "platform/windows/assert.h"

module mini.windows:handle;

import mini.windows;
import mini.platform;
import mini.options;
import mini.engine;
import mini.d3d12;

namespace mini::windows
{

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Handle::Handle()
{
    m_Instance = GetModuleHandleW(nullptr);
    ASSERT(m_Instance);
}

Handle::~Handle()
{
}

bool Handle::Initialize()
{
    auto className = StringConverter<char, wchar_t>(mini::options::name);

    WNDCLASSEXW wcex =
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = m_Instance,
        .hIcon = LoadIconW(m_Instance, MAKEINTRESOURCEW(IDI_DIRECTX12)),
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = MAKEINTRESOURCEW(IDC_DIRECTX12),
        .lpszClassName = className.Data(),
        .hIconSm = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_SMALL)),
    };

    RegisterClassExW(&wcex);

    return true;
}

platform::Window* Handle::CreatePlatformWindow()
{
    return new windows::Window();
}

graphics::Device* Handle::CreateGraphicDevice(graphics::API api)
{
    switch (api)
    {
        case graphics::API::D3D12: return new d3d12::Device();
    }

    return nullptr;
}

void Handle::PollEvents()
{
    MSG msg{};

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT)
        {
            Engine::Quit();
        }
    }
}

void Handle::ProcessMessage([[maybe_unused]] HWND hWnd,
                            [[maybe_unused]] uint32 msg,
                            [[maybe_unused]] WPARAM wParam,
                            [[maybe_unused]] LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                Engine::Abort("Abort!");
            }
            break;

        case WM_SIZE:
            Graphics::ChangeResolution(LOWORD(lParam), HIWORD(lParam), false);
            break;

        default: return;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    windows::Handle* handle = Platform::GetHandle<windows::Handle>();
    handle->ProcessMessage(hWnd, message, wParam, lParam);
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

} // namespace mini::windows