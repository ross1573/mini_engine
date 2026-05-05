module mini.metal4:common;

import mini.core;
import mini.graphics;

namespace mini::metal4 {

constexpr MTL::PrimitiveType GetMTLPrimitiveType(graphics::PrimitiveType primitiveType) noexcept
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