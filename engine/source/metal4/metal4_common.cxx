export module mini.metal4:common;

import mini.core;
import mini.apple;
import mini.graphics;

namespace mini::metal4 {

template <typename T>
concept LabeledTypeT = requires(T* ptr, NS::String* str) { ptr->setLabel(str); };

template <LabeledTypeT T>
String SetLabel(SharedPtr<T>& target, StringView name)
{
    if (name.Empty()) {
        target->setLabel(nullptr);
        return String();
    }

    SharedPtr<NS::String> label = ToNSString(name);
    target->setLabel(label.Get());
    if (label.Valid() == false) [[unlikely]] {
        return String();
    }

    return name;
}

constexpr MTL::PrimitiveType MTLPrimitiveType(graphics::PrimitiveType primitiveType) noexcept
{
    switch (primitiveType) {
        case graphics::PrimitiveType::Point:    return MTL::PrimitiveType::PrimitiveTypePoint;
        case graphics::PrimitiveType::Line:     return MTL::PrimitiveType::PrimitiveTypeLine;
        case graphics::PrimitiveType::Triangle: return MTL::PrimitiveType::PrimitiveTypeTriangle;
    }

    ASSERT(primitiveType != graphics::PrimitiveType::Triangle, "invalid primitive type.");
    return MTL::PrimitiveType::PrimitiveTypeTriangle;
}

} // namespace mini::metal4