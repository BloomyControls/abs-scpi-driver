cmake_minimum_required(VERSION 3.14)

project("ABS SCPI Driver" VERSION 0.0.1)

include(FetchContent)

find_package(fmt 10.2.0 QUIET)
if(NOT fmt_FOUND)
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
    GIT_SHALLOW TRUE
  )
  FetchContent_GetProperties(fmt)
  if(NOT fmt_POPULATED)
    message(STATUS "Fetch fmt")
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()

find_package(Boost 1.81.0 COMPONENTS system QUIET)
if (NOT Boost_FOUND)
  set(BOOST_INCLUDE_LIBRARIES system asio)
  set(BOOST_ENABLE_CMAKE ON)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
  endif()
  FetchContent_Declare(Boost
    SYSTEM
    URL https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.tar.gz
    URL_HASH "SHA1=f71f661f893f39b7b82d66c54980349716f874a2"
  )
  FetchContent_GetProperties(Boost)
  if(NOT Boost_POPULATED)
    message(STATUS "Fetch Boost (may take a while)")
    FetchContent_MakeAvailable(Boost)
  endif()
endif()

add_library(bciabs_scpi src/ScpiDriver.cpp)

target_compile_features(bciabs_scpi PUBLIC cxx_std_20)

target_compile_options(bciabs_scpi PRIVATE -Wall -Wextra)

target_include_directories(bciabs_scpi PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(bciabs_scpi PRIVATE fmt::fmt-header-only)
