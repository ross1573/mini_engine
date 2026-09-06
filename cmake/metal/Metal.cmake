list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/metal")

include(CheckLanguage)
include(CMakeDetermineMetalCompiler)

check_language(Metal)
if (NOT CMAKE_Metal_COMPILER)
    message(FATAL_ERROR "metal compiler not found")
endif()

enable_language(Metal)

function (add_metal_library target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "STANDARD" "")

    add_library(${target} MODULE ${arg_UNPARSED_ARGUMENTS})

    set_target_properties(${target} PROPERTIES
        FOLDER shader
        LANGUAGE Metal
        LINKER_LANGUAGE Metal
    )

    if (arg_STANDARD)
        if (arg_STANDARD MATCHES "metal([0-9]+)\.([0-9]+)")
            target_compile_options(${target} PRIVATE "-std=${arg_STANDARD}")
        else()
            message(WARNING "unknown metal version ${arg_STANDARD}")
        endif()
    endif()

    get_output_directory(output_dir RUNTIME "")
    set(bundle_path "${output_dir}/${BUNDLE_OUTPUT_NAME}.app/Contents/Resources/")
    set(bundle_name "$<TARGET_FILE_NAME:${target}>")
    set(bundle_file "${bundle_path}/${bundle_name}")

    add_dependencies(${target} mini.launcher)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${target}>" "${bundle_file}"
    )
endfunction()