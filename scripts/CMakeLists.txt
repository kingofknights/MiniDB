add_executable(minidb_benchmark benchmark.cpp)
target_link_libraries(minidb_benchmark 
    minidb_common 
    minidb_storage 
    minidb_catalog 
    minidb_parser 
    minidb_execution
)
