module;

#include "metal_layer.h"
#include "quartzcore.h"

export module mini.metal:swap_chain;

import mini.core;
import mini.graphics;

namespace mini::metal {

export class METAL_API SwapChain final
    : public graphics::SwapChain
    , public CoreLayer {
public:
    SwapChain();

    bool Initialize() final;
    void Present() final;

    void ResizeBackBuffer(uint32, uint32, bool) final;
    void SetBackBufferCount(uint8) final;
    void SetVSync(uint8) final;
    void SetFullScreen(bool) final;

    inline Vector2Int GetBackBufferSize() const final { return Vector2Int(0, 0); } // TODO
    inline uint8 GetBackBufferCount() const final { return 0; }                    // TODO
    inline uint8 GetVSync() const final { return 0; }                              // TODO
    bool GetFullScreen() const final;
};

} // namespace mini::metal