# core module mostly contains the fundamentals and relies heavily on templates
# so instead of selectively exporting the symbols, everything will be exported
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN FALSE)

# logging depeneds on core module, so core module has to do it manually
set(NO_API_LOG ON)

add_module(mini.core)

module_sources(mini.core
PUBLIC
    public/assertion.h
    public/option.h

PUBLIC
    FILE_SET core TYPE CXX_MODULES
    FILES
        core.cxx
        algorithm.cxx
        logger.cxx
        utility.cxx

        container/array.cxx
        container/static_array.cxx
        container/static_queue.cxx

        iterator/array_iterator.cxx
        iterator/circular_iterator.cxx
        iterator/iterator.cxx
        iterator/move_iterator.cxx
        iterator/ptr_iterator.cxx

        math/math.cxx
        math/color.cxx
        math/rect.cxx
        math/rect_int.cxx
        math/vector2.cxx
        math/vector2_int.cxx
        math/vector3.cxx
        math/vector3_int.cxx
        math/vector4.cxx
        math/vertex.cxx

        memory/allocator.cxx
        memory/buffer.cxx
        memory/core_memory.cxx
        memory/deleter.cxx
        memory/shared_ptr.cxx
        memory/static_buffer.cxx
        memory/unique_ptr.cxx

        string/format.cxx
        string/string_convert.cxx
        string/string_utility.cxx
        string/string_view.cxx
        string/string.cxx

        type/core_type.cxx
        type/type_concepts.cxx
        type/type_define.cxx
        type/type_numeric.cxx
        type/type_traits.cxx

        module/module_system.cxx
        module/module_static.cxx

        $<$<PLATFORM_ID:Windows>:windows/win_logger.cxx>
        $<$<PLATFORM_ID:Windows>:windows/win_module.cxx>

        $<$<PLATFORM_ID:Darwin>:macos/macos_logger.cxx>
        $<$<PLATFORM_ID:Darwin>:macos/macos_module.cxx>

PRIVATE
    impl/builtin.h
    impl/assert_util.h
    impl/assert_util.cpp
    impl/module_system.cpp

    $<$<PLATFORM_ID:Windows>:impl/windows/win_assert.cpp>
    $<$<PLATFORM_ID:Windows>:impl/windows/win_logger.cpp>
    $<$<PLATFORM_ID:Windows>:impl/windows/win_module.cpp>

    $<$<PLATFORM_ID:Darwin>:impl/macos/macos_assert.cpp>
    $<$<PLATFORM_ID:Darwin>:impl/macos/macos_logger.cpp>
    $<$<PLATFORM_ID:Darwin>:impl/macos/macos_module.cpp>
)

target_precompile_headers(mini.core 
PUBLIC
    public/assertion.h
    public/option.h
)

target_include_directories(mini.core 
PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/impl
)

target_link_libraries(mini.core 
PUBLIC
    convert_utf
    fmt::fmt
)

if (APPLE)
    target_link_libraries(mini.core PRIVATE "-framework Foundation")
endif()

if (MSVC)
    target_compile_options(mini.core PRIVATE /wd4582)
endif()