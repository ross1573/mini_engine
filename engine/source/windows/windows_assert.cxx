export module mini.windows:assert;

namespace mini::debug {

export WINDOWS_API inline constexpr bool Evaluate(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

export WINDOWS_API inline constexpr bool Evaluate(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}

} // namespace mini::debug