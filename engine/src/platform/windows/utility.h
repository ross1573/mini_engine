#pragma once

#include "core/utility.h"
#include "platform/windows/shared_ptr.h"
#include "platform/windows/string_convert.h"

namespace mini::detail
{

bool EnsureHelper(char const* expr, ID3DBlob* error, std::source_location const& loc);

constexpr bool TestExpr(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

constexpr bool TestExpr(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}

} // namespace mini::detail