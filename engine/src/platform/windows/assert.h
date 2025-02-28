#pragma once

#include <d3d12.h>
#include <source_location>
#include "core/assert.h"

namespace mini::detail
{

void EnsureHelper(char const*, ID3DBlob*, std::source_location const&);

constexpr bool TestExpr(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

constexpr bool TestExpr(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}

} // namespace mini::detail