export module mini.d3d12:assert;

import mini.core;

namespace mini::debug {

export template <typename... Args>
AssertFormatResult FormatAssert(char const* _, ID3DBlob* error, AssertFormatContext ctx, Args&&... args)
{
    char const* buffer = reinterpret_cast<char*>(error->GetBufferPointer());
    FormatAssert("d3derror", buffer, ctx, ForwardArg<Args>(args)...);
}

} // namespace mini::debug