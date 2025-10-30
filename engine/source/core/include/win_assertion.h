#pragma once

#if defined(_MINWINDEF_)

inline constexpr bool TestExpr(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

inline constexpr bool TestExpr(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}

#endif

#if defined(__d3dcommon_h__)

ASSERT_API void EnsureHelper(char const*, ID3DBlob*,
                             std::source_location const& = std::source_location::current());

#endif