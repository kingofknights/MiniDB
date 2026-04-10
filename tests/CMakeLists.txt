# Basic test setup
add_executable(unit_tests
    sample_test.cpp
    storage_test.cpp
    record_test.cpp
    table_test.cpp
    parser_test.cpp
    integration_test.cpp
    catalog_test.cpp
    index_test.cpp
    extensive_storage_test.cpp
    extensive_parser_test.cpp
    extensive_execution_test.cpp
    extensive_common_test.cpp
)

target_link_libraries(unit_tests
    GTest::gtest_main
    minidb_common
    minidb_storage
    minidb_catalog
    minidb_parser
    minidb_execution
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
