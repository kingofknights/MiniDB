add_library(minidb_network
    server.cpp
    client.cpp
)

target_link_libraries(minidb_network
    PUBLIC minidb_common minidb_storage minidb_catalog minidb_parser minidb_execution
)

target_include_directories(minidb_network PUBLIC ${CMAKE_SOURCE_DIR})
