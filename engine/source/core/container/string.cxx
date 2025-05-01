module;

#include <format>
#include <string>
#include <string_view>

export module mini.core:string;

import :type;

export namespace mini {

using String = std::basic_string<char>;

} // namespace mini