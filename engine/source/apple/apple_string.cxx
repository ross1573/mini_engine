export module mini.apple:string;

import mini.core;
import :foundation;
import :shared_ptr;

namespace mini {

export APPLE_API SharedPtr<NS::String> toNSString(StringView string)
{
    NS::String* result = NS::String::alloc();
    result = result->init(static_cast<void*>(const_cast<char*>(string.data())),
                          static_cast<NS::UInteger>(string.size()),
                          NS::StringEncoding::UTF8StringEncoding,
                          false);

    ENSURE(result, "failed to initialize NS::String") {
        return nullptr;
    }

    return transfer(result);
}

export APPLE_API String toString(NS::String* string)
{
    if (string == nullptr || string->length() == 0) {
        return {};
    }

    char const* str = string->utf8String();
    return {str};
}

} // namespace mini