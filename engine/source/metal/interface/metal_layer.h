#pragma once

#include "quartzcore.h"

namespace mini::metal {

class CoreLayer {
protected:
    CAMetalLayer* m_Layer;

public:
    ~CoreLayer();

    inline CAMetalLayer* GetMetalLayer() { return m_Layer; }

protected:
    CoreLayer();
};

} // namespace mini::metal