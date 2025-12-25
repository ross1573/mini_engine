export module mini.core:ignore;

import :type;

namespace mini {

struct Ignore {
public:
    template <typename T>
    constexpr Ignore const& operator=(T const&) const noexcept
    {
        return *this;
    }
};

export CORE_API constexpr Ignore ignore;

}; // namespace mini