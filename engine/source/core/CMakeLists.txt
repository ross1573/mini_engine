# some template functions are not exported while using std::format
add_module(mini.core STATIC)

module_sources(mini.core
PUBLIC
    public/assertion.h
    public/option.h

PRIVATE
    assert.cpp

    $<$<PLATFORM_ID:Windows>:windows/win_log.cpp>
    $<$<PLATFORM_ID:Windows>:windows/win_assert.cpp>

    $<$<PLATFORM_ID:Darwin>:macos/macos_log.cpp>
    $<$<PLATFORM_ID:Darwin>:macos/macos_assert.cpp>

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
        container/string.cxx
        container/string_convert.cxx

        iterator/array_iterator.cxx
        iterator/circular_iterator.cxx
        iterator/iterator.cxx
        iterator/iterator_version.cxx
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

        type/core_type.cxx
        type/type_concepts.cxx
        type/type_define.cxx
        type/type_traits.cxx
)

target_link_libraries(mini.core 
PRIVATE 
    convert_utf
)

target_include_directories(mini.core 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/public/
)

if (MSVC)
    target_compile_options(mini.core
    PRIVATE 
        /wd4582)
    # set_source_files_properties(memory/shared_ptr.cxx PROPERTIES COMPILE_FLAGS /wd4582)
    # set_source_files_properties(math/rect.cxx PROPERTIES COMPILE_FLAGS /wd4582)
endif()