export module mini.apple:string;

import mini.core;
import :shared_ptr;

namespace mini {

export APPLE_API SharedPtr<NS::String> ToNSString(StringView string)
{
    NS::String* result = NS::String::alloc();
    ENSURE(result, "failed to allocate NS::String") {
        return nullptr;
    }

    result = result->init(static_cast<void*>(const_cast<char*>(string.Data())),
                          static_cast<NS::UInteger>(string.Size()),
                          NS::StringEncoding::UTF8StringEncoding,
                          false);

    ENSURE(result, "failed to initialize NS::String") {
        return nullptr;
    }

    return TransferShared(result);
}

export APPLE_API String ToString(SharedPtr<NS::String> const& string)
{
    if (string == nullptr || string->length() == 0) {
        return String();
    }

    char const* str = string->utf8String();
    return String(str);
}

} // namespace mini