add_library(minidb_storage
    pager.cpp
    record.cpp
    table_heap.cpp
)

target_link_libraries(minidb_storage
    PUBLIC minidb_common
)

target_include_directories(minidb_storage PUBLIC ${CMAKE_SOURCE_DIR})
