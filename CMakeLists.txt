cmake_minimum_required(VERSION 3.15)
project(CacheForge)

set(CMAKE_CXX_STANDARD 17)

add_executable(cacheforge
    src/main.cpp
    src/disk_manager.cpp
    src/page.cpp
    src/hash_index.cpp
    src/wal.cpp
    src/storage_engine.cpp
)