#include "metal_layer.h"

namespace mini::metal {

CoreLayer::CoreLayer()
    : m_Layer(nullptr)
{
    m_Layer = [[CAMetalLayer alloc] init];
}

CoreLayer::~CoreLayer()
{
    [m_Layer release];
}

} // namespace mini::metal