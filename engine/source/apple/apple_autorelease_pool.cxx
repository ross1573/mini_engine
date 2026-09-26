module;

#include <Foundation/NSAutoreleasePool.hpp>

export module mini.apple:autorelease_pool;

import :shared_ptr;

export namespace NS {

using NS::AutoreleasePool;

} // namespace NS

namespace mini::apple {

export class APPLE_API AutoreleasePool {
private:
    SharedPtr<NS::AutoreleasePool> m_autoreleasePool;

public:
    AutoreleasePool() noexcept
        : m_autoreleasePool(TransferShared(NS::AutoreleasePool::alloc()))
    {
        ENSURE(m_autoreleasePool, "failed to create auto release pool score") {
            return;
        }

        m_autoreleasePool->init();
    }

    void Reset() { m_autoreleasePool.Reset(); }
};

} // namespace mini::apple