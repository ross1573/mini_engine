if (MSVC)
    add_module(mini.core STATIC)
else()
    add_module(mini.core SHARED)
endif()

module_sources(mini.core
PUBLIC
    public/assertion.h
    public/option.h

PUBLIC
    FILE_SET core TYPE CXX_MODULES
    FILES
        core.cxx
        algorithm.cxx
        log.cxx
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
        type/type_traits.cxx

PRIVATE
    impl/assert.cpp

    $<$<PLATFORM_ID:Windows>:windows/win_log.cpp>
    $<$<PLATFORM_ID:Windows>:windows/win_assert.cpp>

    $<$<PLATFORM_ID:Darwin>:macos/macos_log.mm>
    $<$<PLATFORM_ID:Darwin>:macos/macos_log.cpp>
    $<$<PLATFORM_ID:Darwin>:macos/macos_assert.cpp>
)

target_include_directories(mini.core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/impl)
target_link_libraries(mini.core PRIVATE convert_utf)

if (APPLE)
    target_link_libraries(mini.core PRIVATE "-framework Foundation")
endif()

if (MSVC)
    target_compile_options(mini.core PRIVATE /wd4582)
endif()