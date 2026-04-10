# Common library for shared types and utilities
add_library(minidb_common INTERFACE)
target_include_directories(minidb_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/common)

# Sub-components as libraries
add_subdirectory(storage)
add_subdirectory(catalog)
add_subdirectory(parser)
add_subdirectory(execution)
add_subdirectory(network)

# Main REPL/Executable
add_executable(minidb_repl repl/main.cpp)
target_link_libraries(minidb_repl 
    minidb_common 
    minidb_storage
    minidb_catalog 
    minidb_parser 
    minidb_execution
)

# Server and Client executables
add_executable(minidb_server server_main.cpp)
target_link_libraries(minidb_server minidb_network)

add_executable(minidb_client client_main.cpp)
target_link_libraries(minidb_client minidb_network)
