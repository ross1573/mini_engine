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
    AutoreleasePool() noexcept;

    void reset() { m_autoreleasePool.reset(); }
};

AutoreleasePool::AutoreleasePool() noexcept
    : m_autoreleasePool(transfer(NS::AutoreleasePool::alloc()->init()))
{
    m_autoreleasePool->init();
}

} // namespace mini::apple