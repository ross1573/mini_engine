#pragma once

#include "quartzcore.h"

namespace mini::metal {

class CoreLayer {
private:
    CAMetalLayer* m_Layer;

public:
    CoreLayer();

    inline CAMetalLayer* GetMetalLayer() { return m_Layer; }
};

} // namespace mini::metal