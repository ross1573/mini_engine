if (BUILD_TARGET_ARCH MATCHES "[Aa][Rr][Mm]64")
    set(D3D12_ARCH_DIR "arm64")
elseif (BUILD_TARGET_ARCH MATCHES "[Aa][Mm][Dd]64|[Ii][Aa]64")
    set(D3D12_ARCH_DIR "x64")
elseif (BUILD_TARGET_ARCH MATCHES "[Xx]86")
    set(D3D12_ARCH_DIR "win32")
else()
    message(FATAL_ERROR "unsupproted arch: " ${BUILD_TARGET_ARCH})
endif()

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (is_multi_config)
    set(output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

function (copy_d3d12 target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ENGINE_EXTERNAL_DIR}/d3d12/${D3D12_ARCH_DIR}/D3D12Core.dll"
            "$<$<CONFIG:Debug>:${ENGINE_EXTERNAL_DIR}/d3d12/${D3D12_ARCH_DIR}/D3D12Core.pdb>"
            "$<$<CONFIG:Debug>:${ENGINE_EXTERNAL_DIR}/d3d12/${D3D12_ARCH_DIR}/d3d12SDKLayers.dll>"
            "$<$<CONFIG:Debug>:${ENGINE_EXTERNAL_DIR}/d3d12/${D3D12_ARCH_DIR}/d3d12SDKLayers.pdb>"
            "${output_dir}"
        COMMAND_EXPAND_LISTS
    )
endfunction()