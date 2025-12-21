export module mini.utility:ignore;

import mini.type;

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