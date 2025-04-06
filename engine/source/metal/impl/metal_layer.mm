#include "metal_layer.h"

namespace mini::metal {

CoreLayer::CoreLayer()
{
    m_Layer = [[CAMetalLayer alloc] init];
}

} // namespace mini::metal