#pragma once

#include "core/utility.h"
#include "platform/log.h"
#include "platform/windows/shared_ptr.h"
#include "platform/windows/string_convert.h"

namespace mini::detail
{

constexpr bool TestExpr(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

constexpr bool TestExpr(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}

constexpr bool TestExpr(HRESULT result, ID3DBlob* error)
{
    if (FAILED(result))
    {
        wchar_t* buffer = (wchar_t*)error->GetBufferPointer();
        Log::Error(windows::StringConverter<wchar_t, char>(buffer).Data());
        return false;
    }

    return true;
}

} // namespace mini::detail