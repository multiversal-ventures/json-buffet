cmake_minimum_required(VERSION 3.14)

project(json-buffet LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(json-buffet
  main.cpp
  jsonbuffet.cpp
  jsonbuffet.h
)

target_include_directories(json-buffet PRIVATE thirdparty/rapidjson/include/)
target_compile_definitions(json-buffet PRIVATE -DRAPIDJSON_HAS_STDSTRING=1 -DRAPIDJSON_SSE42=1)
