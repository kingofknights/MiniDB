add_library(minidb_execution
    executor.cpp
)

target_link_libraries(minidb_execution
    PUBLIC minidb_common minidb_storage minidb_catalog minidb_parser
)

target_include_directories(minidb_execution PUBLIC ${CMAKE_SOURCE_DIR})
