#ifndef WIN_ASSERT_H
#define WIN_ASSERT_H

#if defined(_MINWINDEF_)
inline constexpr bool TestExpr(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

inline constexpr bool TestExpr(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}
#endif // defined(_MINWINDEF_)

#if defined(__d3dcommon_h__)
ASSERT_API void EnsureHelper(char const*, ID3DBlob*,
                             std::source_location const& = std::source_location::current());
#endif // defined(__d3dcommon_h__)

#endif // WIN_ASSERT_H