if (MSVC)
    add_executable(${PROJECT_NAME} WIN32
        win_main.cpp
        launcher.cxx   
    )
else()
    message(FATAL_ERROR "unable to build on unknown platform")
endif()

target_link_libraries(${PROJECT_NAME}
PRIVATE 
    mini.core
    mini.engine
    mini.windows
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()