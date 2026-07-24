cmake_minimum_required(VERSION 3.6...4.3)
PROJECT(Job-scraper LANGUAGES CXX)
include(FetchContent)




set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 1.14.2)
FetchContent_MakeAvailable(cpr)

add_executable(job-scraper main.cpp)

target_link_libraries(job-scraper PRIVATE cpr::cpr)