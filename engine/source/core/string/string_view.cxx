module;

#include <string_view>

export module mini.core:string_view;

namespace mini {

export using StringView = std::basic_string_view<char>;

} // namespace mini