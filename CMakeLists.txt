cmake_minimum_required(VERSION 3.6...4.3)
if(CMAKE_HOST_UNIX AND NOT CYGWIN AND NOT MINGW)
    # This prevents path conversion issues
endif()
set(BUILD_SHARED_LIBS OFF)
set(CPR_CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
PROJECT(Job-scraper LANGUAGES CXX C)
include(FetchContent)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/final_build)

FetchContent_Declare(cpr 
    GIT_REPOSITORY https://github.com/libcpr/cpr.git 
    GIT_TAG 1.14.2
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    lexbor
    GIT_REPOSITORY https://github.com/lexbor/lexbor.git
    GIT_TAG v2.5.0
)

FetchContent_MakeAvailable(lexbor)

add_executable(job-scraper main.cpp parser.cpp)

target_link_libraries(job-scraper PRIVATE 
    cpr::cpr 
    lexbor
)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/final_build)