add_module(mini.core 
    NO_API_LOG
    NO_MODULE_INTERFACE
)

generate_api_header(mini.core PUBLIC PREFIX mini API assert)

target_sources(mini.core
PUBLIC
    include/assertion.h
    include/option.h

PUBLIC
    FILE_SET core TYPE CXX_MODULES
    FILES
        core.cxx
        algorithm.cxx
        logger.cxx

        # chrono/clock.cxx
        chrono/duration.cxx

        container/array.cxx
        container/static_array.cxx
        container/static_queue.cxx

        iterator/array_iterator.cxx
        iterator/circular_iterator.cxx
        iterator/iterator.cxx
        iterator/move_iterator.cxx
        iterator/ptr_iterator.cxx

        math/math.cxx
        math/math_base.cxx
        math/ratio.cxx
        math/color.cxx
        math/rect.cxx
        math/rect_int.cxx
        math/vector2.cxx
        math/vector2_int.cxx
        math/vector3.cxx
        math/vector3_int.cxx
        math/vector4.cxx
        math/vertex.cxx

        memory/atomic.cxx
        memory/atomic_base.cxx
        memory/atomic_wait.cxx
        memory/allocator.cxx
        memory/buffer.cxx
        memory/memory.cxx
        memory/deleter.cxx
        memory/shared_counter.cxx
        memory/shared_ptr.cxx
        memory/static_buffer.cxx
        memory/unique_ptr.cxx

        module/module_system.cxx
        module/module_static.cxx

        string/format.cxx
        string/string_convert.cxx
        string/string_utility.cxx
        string/string_view.cxx
        string/string.cxx

        thread/mutex.cxx

        type/core_type.cxx
        type/type_concepts.cxx
        type/type_define.cxx
        type/type_numeric.cxx
        type/type_traits.cxx

        utility/utility.cxx
        utility/bit.cxx
        utility/bit_base.cxx

        $<$<PLATFORM_ID:Windows>:windows/win_atomic.cxx>
        $<$<PLATFORM_ID:Windows>:windows/win_bit.cxx>
        $<$<PLATFORM_ID:Windows>:windows/win_logger.cxx>
        $<$<PLATFORM_ID:Windows>:windows/win_module.cxx>
        $<$<PLATFORM_ID:Windows>:windows/win_mutex.cxx>
        $<$<PLATFORM_ID:Windows>:windows/win_thread.cxx>

        $<$<PLATFORM_ID:Darwin>:macos/macos_atomic.cxx>
        $<$<PLATFORM_ID:Darwin>:macos/macos_logger.cxx>
        $<$<PLATFORM_ID:Darwin>:macos/macos_module.cxx>
        $<$<PLATFORM_ID:Darwin>:macos/macos_mutex.cxx>
        $<$<PLATFORM_ID:Darwin>:macos/macos_thread.cxx>

PRIVATE
    impl/assert_util.h
    impl/assert_util.cpp
    impl/module_system.cpp

    math/cmath.h

    memory/cstring.h
    memory/cwstring.h

    utility/bit.h

    $<$<CXX_COMPILER_ID:MSVC>:impl/windows/win_atomic.cpp>
    $<$<PLATFORM_ID:Windows>:include/win_assert.h>
    $<$<PLATFORM_ID:Windows>:impl/windows/win_assert.cpp>
    $<$<PLATFORM_ID:Windows>:impl/windows/win_logger.cpp>
    $<$<PLATFORM_ID:Windows>:impl/windows/win_module.cpp>

    $<$<PLATFORM_ID:Darwin>:impl/macos/macos_assert.cpp>
    $<$<PLATFORM_ID:Darwin>:impl/macos/macos_logger.cpp>
    $<$<PLATFORM_ID:Darwin>:impl/macos/macos_module.cpp>
)

target_precompile_headers(mini.core 
PUBLIC
    include/assertion.h
    include/option.h
)

target_link_libraries(mini.core 
PUBLIC
    convert_utf
    fmt::fmt
)

if (APPLE)
    target_link_libraries(mini.core PRIVATE "-framework Foundation")
elseif (WIN32)
    target_link_libraries(mini.core PRIVATE synchronization.lib)
endif()

if (MSVC)
    target_compile_options(mini.core PRIVATE /wd4582)
endif()

build_source_tree(mini.core)