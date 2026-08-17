include(mini_util)

function (_macos_bundle_module target)
    get_output_directory(output_dir RUNTIME "")
    set(bundle_path "${output_dir}/${BUNDLE_OUTPUT_NAME}.app/Contents/Frameworks")
    set(bundle_name $<TARGET_FILE_NAME:${target}>)
    set(bundle_file ${bundle_path}/${bundle_name})

    set_target_properties(${target} PROPERTIES
        BUILD_RPATH "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
    
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> "${bundle_file}"
        COMMENT "Copying ${bundle_name} to ${bundle_file}"
    )
endfunction()

function (_macos_set_exec_properties target)
    set_target_properties(${target} 
    PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${ENGINE_PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${ENGINE_PROJECT_VERSION}
        MACOSX_BUNDLE_COPYRIGHT ${ENGINE_PROJECT_AUTHOR}
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.${ENGINE_PROJECT_AUTHOR}.${ENGINE_PROJECT_NAME}"

        BUILD_RPATH "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endfunction()

function (bundle_module target)
    if (BUILD_MONOLITHIC)
        return()
    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        _macos_bundle_module(${target})
    endif()
endfunction()

function (relocate_executable target)
    if (BUILD_MONOLITHIC)
        return()
    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        _macos_set_exec_properties(${target})
    endif()
endfunction()