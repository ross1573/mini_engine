add_module(mini.core 
    NO_API_LOG
    NO_MODULE_INTERFACE
)

generate_api_header(mini.core PUBLIC PREFIX mini API assert)

# essentials
target_sources(mini.core
PUBLIC
    FILE_SET type TYPE CXX_MODULES
    FILES
        type/type_define.cxx
        type/type_traits.cxx
        type/type_concepts.cxx
        type/type_numeric.cxx
        type/type_operation.cxx
        type/type.cxx
)
target_sources(mini.core
PUBLIC
    FILE_SET utility TYPE CXX_MODULES
    FILES
        utility/ignore.cxx
        utility/initializer_list.cxx
        utility/source_location.cxx
        utility/shared_counter.cxx
        utility/shared_ptr.cxx
        utility/unique_ptr.cxx
        utility/utility.cxx
)

# memory
target_sources(mini.core
PUBLIC
    FILE_SET memory TYPE CXX_MODULES
    FILES
        memory/memory_operation.cxx
        memory/allocator.cxx
        memory/deleter.cxx

        memory/dynamic_buffer.cxx
        memory/trivial_buffer.cxx
        memory/static_buffer.cxx
        
        memory/memory.cxx
)
target_sources(mini.core
PUBLIC
    FILE_SET bit TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:bit/bit_win.cxx>
        bit/bit_base.cxx
        bit/bit_operation.cxx
        bit/bit.cxx
)
target_sources(mini.core
PUBLIC
    FILE_SET atomic TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:atomic/atomic_win.cxx>
        $<$<PLATFORM_ID:Darwin>:atomic/atomic_macos.cxx>
)

# math
target_sources(mini.core
PUBLIC
    FILE_SET math TYPE CXX_MODULES
    FILES
        math/math_base.cxx
        math/math_type.cxx
        math/math_operation.cxx
        math/ratio.cxx
        math/rect.cxx
        math/rect_int.cxx
        math/vector2.cxx
        math/vector2_int.cxx
        math/vector3.cxx
        math/vector3_int.cxx
        math/vector4.cxx
        math/color.cxx
        math/math.cxx
)

# container
target_sources(mini.core
PUBLIC
    FILE_SET iterator TYPE CXX_MODULES
    FILES
        iterator/iterator_type.cxx
        iterator/iterator_std.cxx
        iterator/move_iterator.cxx
        iterator/ptr_iterator.cxx
        iterator/iterator.cxx
)
target_sources(mini.core
PUBLIC
    FILE_SET array TYPE CXX_MODULES
    FILES
        array/dynamic_array.cxx
        array/static_array.cxx
        array/array_iterator.cxx
        array/array.cxx
)
target_sources(mini.core
PUBLIC
    FILE_SET queue TYPE CXX_MODULES
    FILES
        queue/static_queue.cxx 
        queue/circular_iterator.cxx
        queue/queue.cxx
)

# string
target_sources(mini.core
PUBLIC
    FILE_SET string TYPE CXX_MODULES
    FILES
        string/string_memory.cxx
        string/string_iterator.cxx
        string/basic_string.cxx
        string/basic_string_view.cxx
        string/basic_string_convert.cxx
        string/format.cxx
        string/string.cxx
)

# algorithm
target_sources(mini.core
PUBLIC
    FILE_SET algorithm TYPE CXX_MODULES
    FILES
        algorithm/algorithm.cxx
)

# chrono
target_sources(mini.core
PUBLIC
    FILE_SET chrono TYPE CXX_MODULES
    FILES
        chrono/duration.cxx
        chrono/chrono.cxx
)

# thread
target_sources(mini.core
PUBLIC
    FILE_SET thread TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:thread/atomic_win.cxx>
        $<$<PLATFORM_ID:Darwin>:thread/atomic_macos.cxx>
        thread/atomic_base.cxx
        thread/atomic_wait.cxx
        thread/atomic.cxx

        $<$<PLATFORM_ID:Windows>:thread/mutex_win.cxx>
        $<$<PLATFORM_ID:Darwin>:thread/mutex_macos.cxx>
        thread/mutex.cxx

        thread/thread.cxx

PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:impl/windows/win_atomic.cpp>
)

# modules
target_sources(mini.core
PUBLIC
    FILE_SET module TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:module/module_win.cxx>
        $<$<PLATFORM_ID:Darwin>:module/module_macos.cxx>
        module/module_system.cxx
        module/static_module.cxx
        module/module.cxx

PRIVATE
    $<$<PLATFORM_ID:Windows>:module/impl/module_win.cpp>
    $<$<PLATFORM_ID:Darwin>:module/impl/module_macos.cpp>
    module/impl/module_system.cpp
)

# debug
target_sources(mini.core
PUBLIC
    FILE_SET debug TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:debug/logger_win.cxx>
        $<$<PLATFORM_ID:Darwin>:debug/logger_macos.cxx>
        debug/logger.cxx
        debug/debug.cxx

PRIVATE
    $<$<PLATFORM_ID:Windows>:debug/impl/logger_win.cpp>
    $<$<PLATFORM_ID:Darwin>:debug/impl/logger_macos.cpp>
    $<$<PLATFORM_ID:Windows>:debug/impl/assert_win.cpp>
    $<$<PLATFORM_ID:Darwin>:debug/impl/assert_macos.cpp>
    debug/impl/assert_util.cpp
)

# core
target_sources(mini.core
PUBLIC
    FILE_SET core TYPE CXX_MODULES
    FILES
        core.cxx
)

target_precompile_headers(mini.core 
PUBLIC
    debug/include/assertion.h
    include/option.h
)

target_include_directories(mini.core
PUBLIC
    debug/include
)

target_link_libraries(mini.core 
PUBLIC
    convert_utf
    fmt::fmt
)

if (APPLE)
    target_link_libraries(mini.core PRIVATE "-framework CoreFoundation")
elseif (WIN32)
    target_link_libraries(mini.core PRIVATE synchronization.lib)
endif()

if (MSVC)
    target_compile_options(mini.core PRIVATE /wd4582)
endif()

build_source_tree(mini.core)