module;

#include <source_location>

export module mini.utility:source_location;

namespace mini {

// yet another compiler's black magic
// msvc does it with naive approach, which takes up to 24 bytes or more
// clang and gcc on the other hand, achieve's it with only 8 bytes (with single ptr)
//
// we can just impl it with msvc's approach, but it will chip away some perf
// unless the abi is seriously considered, we're just gonna leave it as it is
export using SourceLocation = std::source_location;

} // namespace mini