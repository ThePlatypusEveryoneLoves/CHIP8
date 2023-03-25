cmake_minimum_required (VERSION 3.16.0)
project (main CXX)
include(FetchContent)

SET(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES  ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") 
#get ftxui
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v4.0.0)
  
FetchContent_MakeAvailable(ftxui)

include_directories(include)

file(GLOB SRC src/*.cpp src/*.txt)
add_executable(main ${SRC})

#include ftxui library
target_link_libraries(main ftxui::component ftxui::dom ftxui::screen)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/memory.txt
    ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
set (CMAKE_CXX_STANDARD 17)
#linking options
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
