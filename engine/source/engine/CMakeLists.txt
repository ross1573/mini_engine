add_module(mini.engine SHARED)

module_sources(mini.engine
PUBLIC
    FILE_SET engine TYPE CXX_MODULES
    FILES
        engine.cxx
)

target_sources(mini.engine
PRIVATE
    engine.cpp
)

target_link_libraries(mini.engine
PRIVATE
    mini.core
    mini.platform
    mini.graphics
)