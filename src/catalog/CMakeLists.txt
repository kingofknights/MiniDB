add_library(minidb_catalog
    catalog.cpp
)

target_link_libraries(minidb_catalog
    PUBLIC minidb_common
)

target_include_directories(minidb_catalog PUBLIC ${CMAKE_SOURCE_DIR})
