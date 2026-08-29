export module mini.apple:assert;

import mini.core;

namespace mini::debug {

extern "C++" {
size_t FormatHex(char* dest, char const* destEnd, uint64 src) noexcept;
size_t FormatAssertLine(char* dest, char const* destEnd, char const** src, size_t count) noexcept;
size_t FormatAssertString(char* dest, char const* destEnd, char const* src) noexcept;
}

constexpr size_t errorBufferSize = 1024;
constexpr char const* nserrorReason = "- reason: ";
constexpr char const* nserrorDesc = "- description: ";
constexpr char const* nserrorRecovery = "- recovery: ";

APPLE_API void FormatNSError(char* dest, char const* destEnd, NS::Error* error)
{
    size_t code;
    char hexBuffer[18];
    char const* buffer[4];
    char const* domain;
    char const* reason;
    char const* desc;
    char const* recovery;

    if (error == nullptr) [[unlikely]] {
        goto format_nserror_result;
    }

    code = static_cast<size_t>(error->code());
    domain = error->domain()->utf8String();
    reason = error->localizedFailureReason()->utf8String();
    desc = error->localizedDescription()->utf8String();
    recovery = error->localizedRecoverySuggestion()->utf8String();

    FormatHex(hexBuffer, hexBuffer + sizeof(hexBuffer), code);

    buffer[0] = domain;
    buffer[1] = " (";
    buffer[2] = hexBuffer;
    buffer[3] = ")";
    dest += FormatAssertLine(dest, destEnd, buffer, 4);
    if (dest == destEnd) [[unlikely]] {
        goto format_nserror_result;
    }

    buffer[0] = nserrorReason;
    buffer[1] = reason;
    dest += FormatAssertLine(dest, destEnd, buffer, 2);
    if (dest == destEnd) [[unlikely]] {
        goto format_nserror_result;
    }

    buffer[0] = nserrorDesc;
    buffer[1] = desc;
    dest += FormatAssertLine(dest, destEnd, buffer, 2);
    if (dest == destEnd) [[unlikely]] {
        goto format_nserror_result;
    }

    buffer[0] = nserrorRecovery;
    buffer[1] = recovery;
    dest += FormatAssertLine(dest, destEnd, buffer, 2);
    if (dest == destEnd) [[unlikely]] {
        goto format_nserror_result;
    }

format_nserror_result:
    *dest = '\0';
    return;
}

export template <typename... Args>
AssertFormatResult FormatAssert(char const* _, NS::Error* error, AssertFormatContext const& ctx, Args&&... args)
{
    thread_local char errorBuffer[errorBufferSize] = { 0 };

    FormatNSError(errorBuffer, errorBuffer + errorBufferSize, error);
    return FormatAssert("nserror", errorBuffer, ctx, ForwardArg<Args>(args)...);
}

} // namespace mini::debug