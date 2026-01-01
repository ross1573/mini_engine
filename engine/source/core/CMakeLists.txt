add_module(mini.core 
    NO_API_LOG
    NO_MODULE_ENTRY
)

generate_api_header(mini.core PUBLIC PREFIX mini API assert)

target_sources(mini.core
PUBLIC
    FILE_SET meta TYPE CXX_MODULES
    FILES
        meta/type_define.cxx
        meta/type_traits.cxx
        meta/type_concepts.cxx
        meta/type.cxx
        meta/numeric.cxx
        meta/ratio.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET utility TYPE CXX_MODULES
    FILES
        utility/ignore.cxx
        utility/initializer_list.cxx
        utility/source_location.cxx
        utility/utility_operation.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET memory TYPE CXX_MODULES
    FILES
        memory/allocator.cxx
        memory/deleter.cxx
        memory/memory_operation.cxx
        memory/shared_counter.cxx
        memory/shared_ptr.cxx
        memory/unique_ptr.cxx

PRIVATE
    memory/cstring.h
    memory/cwstring.h
    memory/memory.h
)

target_sources(mini.core
PUBLIC
    FILE_SET bit TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:bit/bit_win.cxx>
        bit/bit_base.cxx
        bit/bit_operation.cxx

PRIVATE
    bit/bit.h
)

target_sources(mini.core
PUBLIC
    FILE_SET math TYPE CXX_MODULES
    FILES
        math/math_base.cxx
        math/math_type.cxx
        math/math_operation.cxx

        math/color.cxx
        math/rect.cxx
        math/rect_int.cxx
        math/vector2.cxx
        math/vector2_int.cxx
        math/vector3.cxx
        math/vector3_int.cxx
        math/vector4.cxx

PRIVATE
    math/cmath.h
)

target_sources(mini.core
PUBLIC
    FILE_SET iterator TYPE CXX_MODULES
    FILES
        iterator/iterator.cxx
        iterator/ptr_iterator.cxx
        iterator/move_iterator.cxx
        iterator/array_iterator.cxx
        iterator/circular_iterator.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET container TYPE CXX_MODULES
    FILES
        container/static_buffer.cxx
        container/dynamic_buffer.cxx
        container/trivial_buffer.cxx

        container/array.cxx
        container/static_array.cxx
        container/static_queue.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET string TYPE CXX_MODULES
    FILES
        string/string_memory.cxx
        string/string_convert.cxx
        string/string_view.cxx
        string/string.cxx
        string/format.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET debug TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:debug/logger_win.cxx>
        $<$<PLATFORM_ID:Darwin>:debug/logger_macos.cxx>
        debug/logger.cxx

PRIVATE
    $<$<PLATFORM_ID:Windows>:debug/impl/logger_win.cpp>
    $<$<PLATFORM_ID:Darwin>:debug/impl/logger_macos.cpp>
    $<$<PLATFORM_ID:Windows>:debug/impl/assert_win.cpp>
    $<$<PLATFORM_ID:Darwin>:debug/impl/assert_macos.cpp>
    debug/impl/assert_util.cpp

    $<$<PLATFORM_ID:Windows>:debug/include/assertion_win.h>
    debug/include/assertion.h
)

target_sources(mini.core
PUBLIC
    FILE_SET concurrency TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:concurrency/atomic_platform_win.cxx>
        $<$<PLATFORM_ID:Windows>:concurrency/atomic_wait_win.cxx>
        $<$<PLATFORM_ID:Darwin>:concurrency/atomic_platform_macos.cxx>
        $<$<PLATFORM_ID:Darwin>:concurrency/atomic_wait_macos.cxx>
        concurrency/atomic_base.cxx
        concurrency/atomic_wait.cxx
        concurrency/atomic.cxx

        $<$<PLATFORM_ID:Windows>:concurrency/mutex_win.cxx>
        $<$<PLATFORM_ID:Darwin>:concurrency/mutex_macos.cxx>
        concurrency/mutex.cxx

PRIVATE
    $<$<PLATFORM_ID:Windows>:concurrency/impl/atomic_win.cpp>
)

target_sources(mini.core
PUBLIC
    FILE_SET module TYPE CXX_MODULES
    FILES
        $<$<PLATFORM_ID:Windows>:module/module_win.cxx>
        $<$<PLATFORM_ID:Darwin>:module/module_macos.cxx>
        module/module_system.cxx
        module/module_loader.cxx
        module/dynamic_module.cxx
        module/static_module.cxx

PRIVATE
    module/impl/module_system.cpp
    module/impl/dynamic_module.cpp
)

target_sources(mini.core
PUBLIC
    FILE_SET algorithm TYPE CXX_MODULES
    FILES
        algorithm/algorithm.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET chrono TYPE CXX_MODULES
    FILES
        chrono/duration.cxx
        chrono/time_point.cxx
        $<$<PLATFORM_ID:Windows>:chrono/clock_win.cxx>
        $<$<PLATFORM_ID:Darwin>:chrono/clock_macos.cxx>
        chrono/clock.cxx
)

target_sources(mini.core
PUBLIC
    FILE_SET core TYPE CXX_MODULES
    FILES
        core.cxx

PRIVATE
    $<$<PLATFORM_ID:Windows>:internal/min_windows.h>
)

target_precompile_headers(mini.core 
PUBLIC
    debug/include/assertion.h
    include/option.h
)

target_include_directories(mini.core
PUBLIC
    debug/include
    include/
    internal/
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