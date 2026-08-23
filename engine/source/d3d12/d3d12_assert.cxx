export module mini.d3d12:assert;

import mini.core;

namespace mini::debug {

D3D12_API void AssertFormat(char const* expr, ID3DBlob* error, SourceLocation loc = SourceLocation::current())
{
    AssertFormat(expr, reinterpret_cast<char*>(error->GetBufferPointer()), loc);
}

} // namespace mini::debug