include_guard()
include(FetchContent)

FetchContent_Declare(NuGetCMake
    GIT_REPOSITORY https://github.com/mschofie/NuGetCMakePackage
    GIT_TAG 5f547903030535424f913d1e8153fcd5f85997fa
)
FetchContent_MakeAvailable(NuGetCMake)

set(nuget_config_path "${CMAKE_BINARY_DIR}/nuget.config")
set(nuget_template_path "${ENGINE_PROJECT_DIR}/cmake/template/nuget.config")

if (NOT EXISTS "${nuget_config_path}")
    file(COPY_FILE "${nuget_template_path}" "${nuget_config_path}")
endif()

function (add_nuget_package name)
    set(KEYWORDS PACKAGE VERSION LOCK_FILE FRAMEWORK)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${KEYWORDS}" "")

    if (NOT arg_PACKAGE)
        message(FATAL_ERROR "package undefined")
    endif()

    if (NOT arg_VERSION)
        message(FATAL_ERROR "version undefined")
    endif()

    if (NOT arg_LOCK_FILE)
        set(arg_LOCK_FILE "${CMAKE_CURRENT_BINARY_DIR}/packages.lock")
    endif()

    if (EXISTS ${arg_LOCK_FILE})
        # usually version is managed when the configuration is done
        # and by leaving the lock file, this will corrupt the config process
        # which brings up to a stupid error saying "lock file needs to be DELETED"
        # so, fuck lock file. we dont need this shit hanging around the "from scratch" project
        file(REMOVE ${arg_LOCK_FILE})
    endif()

    if (arg_FRAMEWORK)
        set(OPT_FRAMEWORK "FRAMEWORK ${arg_FRAMEWORK}")
    else()
        set(OPT_FRAMEWORK "")
    endif()

    add_nuget_packages(
        PACKAGES ${arg_PACKAGE} ${arg_VERSION}
        CONFIG_FILE ${nuget_config_path}
        LOCK_FILE ${arg_LOCK_FILE}
        ${OPT_FRAMEWORK}
    )

    string(TOLOWER ${arg_PACKAGE} package_lower)

    # hard coded path to match add_nuget_packages
    # because the stupid library does not tell you the path if the package does not contain any cmake file
    set("${name}_DIR" "${CMAKE_BINARY_DIR}/_deps/${package_lower}/${arg_VERSION}/build" PARENT_SCOPE)
endfunction()