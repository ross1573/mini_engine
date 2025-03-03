# some template functions are not exported while using std::format
add_module(mini.core STATIC)

target_precompile_headers(mini.core
PRIVATE
    assert.h
)

target_sources(mini.core
PUBLIC
    FILE_SET core TYPE CXX_MODULES
    FILES
        core.cxx

PUBLIC
    assert.h
    option.h

PRIVATE
    assert.cpp
    memory/mem_string.cpp

PRIVATE 
    FILE_SET core_private TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        algorithm.cxx
        log.cxx
        utility.cxx

        container/array.cxx
        container/static_array.cxx
        container/static_queue.cxx
        container/string.cxx

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
        memory/mem_string.cxx
        memory/shared_ptr.cxx
        memory/static_buffer.cxx
        memory/unique_ptr.cxx

        type/core_type.cxx
        type/type_concepts.cxx
        type/type_define.cxx
        type/type_traits.cxx
)