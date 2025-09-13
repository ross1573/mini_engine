template <typename T>
struct CStr_inline;

template <typename T>
struct CStr_constexpr;

#define CSTR(type, prefix, ...)                                                           \
    template <>                                                                           \
    struct CStr_##prefix<type> {                                                          \
        static prefix type ch = __VA_ARGS__##'a';                                         \
        static prefix type const e[] = __VA_ARGS__##"";                                   \
        static prefix type const s[] = __VA_ARGS__##"Hello world!";                       \
        static prefix type const l[] = __VA_ARGS__##"Hello world! This is a long string"; \
        static prefix type const s_10[] = __VA_ARGS__##"Hello worl";                      \
        static prefix type const l_30[] = __VA_ARGS__##"Hello world! This is a long st";  \
    }

CSTR(char, inline);
CSTR(wchar_t, inline, L);
CSTR(char8_t, inline, u8);
CSTR(char16_t, inline, u);
CSTR(char32_t, inline, U);

CSTR(char, constexpr);
CSTR(wchar_t, constexpr, L);
CSTR(char8_t, constexpr, u8);
CSTR(char16_t, constexpr, u);
CSTR(char32_t, constexpr, U);

#ifdef DEBUG_TEST_STRING
#  define TEST_STRING(func, type) TEST_ENSURE((func<type, CStr_inline<type>>() == 0));
#else
#  define TEST_STRING(func, type)                             \
      static_assert(func<type, CStr_constexpr<type>>() == 0); \
      TEST_ENSURE((func<type, CStr_inline<type>>() == 0));
#endif