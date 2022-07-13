cmake_minimum_required(VERSION 3.14)

project(json-buffet LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BUFFET_SRCS jsonbuffet.cpp utils.cpp jsonbuffet.h utils.h)
add_executable(json-buffet main.cpp ${JSON_BUFFET_SRCS})
target_include_directories(json-buffet PRIVATE thirdparty/rapidjson/include/)
target_compile_definitions(json-buffet PRIVATE -DRAPIDJSON_HAS_STDSTRING=1 -DRAPIDJSON_SSE42=1)

add_executable(json-chunker json-chunker.cpp ${JSON_BUFFET_SRCS})
target_include_directories(json-chunker PRIVATE thirdparty/rapidjson/include/)
target_compile_definitions(json-chunker PRIVATE -DRAPIDJSON_HAS_STDSTRING=1 -DRAPIDJSON_SSE42=1)

add_executable(json-indexer json-indexer.cpp ${JSON_BUFFET_SRCS})
target_include_directories(json-indexer PRIVATE thirdparty/rapidjson/include/)
target_compile_definitions(json-indexer PRIVATE -DRAPIDJSON_HAS_STDSTRING=1 -DRAPIDJSON_SSE42=1)
