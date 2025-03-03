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