add_library(minidb_parser
    lexer.cpp
    parser.cpp
)

target_link_libraries(minidb_parser
    PUBLIC minidb_common minidb_catalog
)

target_include_directories(minidb_parser PUBLIC ${CMAKE_SOURCE_DIR})
