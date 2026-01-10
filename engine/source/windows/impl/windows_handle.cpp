module;

#include "resource.h"

module mini.windows;

import mini.engine;
import mini.graphics;
import mini.platform;

namespace mini::windows {

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Handle::Handle()
{
    auto className = options::name;

    m_Instance = GetModuleHandle(nullptr);
    ASSERT(m_Instance);

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
}

bool Handle::IsValid() const noexcept
{
    return m_Instance != nullptr;
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

void Handle::AlertError(StringView const& msg)
{
    HWND handle = interface->GetWindow()->GetHWND();
    MessageBoxA(handle, msg.Data(), nullptr, MB_ICONERROR | MB_OK);
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
    Handle* handle = interface->GetHandle();
    handle->ProcessMessage(hWnd, message, wParam, lParam);
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

} // namespace mini::windows