cmake_minimum_required(VERSION 3.14)

project(absscpi
  VERSION 1.0.0
  DESCRIPTION "ABS SCPI driver library"
  HOMEPAGE_URL "https://github.com/BloomyControls/abs-scpi-driver"
)

option(ABSSCPI_INSTALL "Enable install" ON)

if(ABSSCPI_STATIC)
  set(ABSSCPI_LIB_TYPE STATIC)
elseif(BUILD_SHARED_LIBS)
  set(ABSSCPI_LIB_TYPE SHARED)
else()
  set(ABSSCPI_LIB_TYPE STATIC)
endif()

include(FetchContent)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

set(FMT_SYSTEM_HEADERS TRUE)
set(FMT_INSTALL OFF)
find_package(fmt 10.2.1 QUIET)
if(NOT fmt_FOUND)
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
    GIT_SHALLOW TRUE
  )
  FetchContent_GetProperties(fmt)
  message(STATUS "Fetch fmt")
  FetchContent_MakeAvailable(fmt)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(BOOST_INCLUDE_LIBRARIES system asio)
set(BOOST_ENABLE_CMAKE ON)
# use CONFIG to avoid CMP0167 warnings
find_package(Boost 1.81.0 CONFIG COMPONENTS system QUIET)
if (NOT Boost_FOUND)
  set(SAVED_BUILD_SHARED ${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS OFF)
  FetchContent_Declare(Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.tar.gz
    URL_HASH "SHA1=f71f661f893f39b7b82d66c54980349716f874a2"
  )
  FetchContent_GetProperties(Boost)
  if(NOT Boost_POPULATED)
    message(STATUS "Fetch Boost (may take a while)")
    FetchContent_MakeAvailable(Boost)
  endif()
  # workaround an issue with fetched boost asio
  set(BOOST_LIBS ${Boost_LIBRARIES} Boost::asio)
  set(BUILD_SHARED_LIBS ${SAVED_BUILD_SHARED})
else()
  set(BOOST_LIBS ${Boost_LIBRARIES})
endif()

option(FASTFLOAT_INSTALL "" OFF)
FetchContent_Declare(fast_float
  GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
  GIT_TAG v6.1.1
  GIT_SHALLOW TRUE
)
message(STATUS "Fetch fast_float")
FetchContent_MakeAvailable(fast_float)

find_package(Doxygen 1.9.8 OPTIONAL_COMPONENTS dot)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

add_library(absscpi ${ABSSCPI_LIB_TYPE}
  src/TcpDriver.cpp
  src/UdpDriver.cpp
  src/UdpMulticastDriver.cpp
  src/SerialDriver.cpp
  src/ScpiUtil.cpp
  src/ScpiClient.cpp
  src/ScpiClient_System.cpp
  src/ScpiClient_Cells.cpp
  src/ScpiClient_AuxIO.cpp
  src/ScpiClient_Modeling.cpp
  src/Discovery.cpp
  src/Errors.cpp
  src/CInterface.cpp
)
add_library(bci::absscpi ALIAS absscpi)

set_target_properties(absscpi PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  POSITION_INDEPENDENT_CODE ON
)

target_compile_features(absscpi PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(absscpi PRIVATE /W3)
else()
  target_compile_options(absscpi PRIVATE -Wall -Wextra)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(absscpi PRIVATE -Wno-psabi)
  endif()
endif()

if(ABSSCPI_INSTALL AND BUILD_SHARED_LIBS)
  include(GNUInstallDirs)
endif()

target_include_directories(absscpi PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(absscpi PRIVATE
  fmt::fmt-header-only
  fast_float
  ${BOOST_LIBS}
)

if(DOXYGEN_FOUND)
  include(cmake/add_doxygen_target.cmake)
endif()

if(ABSSCPI_INSTALL AND BUILD_SHARED_LIBS)
  include(CMakePackageConfigHelpers)

  set(targets_export_name "absscpi-targets")
  set(version_config "${PROJECT_BINARY_DIR}/absscpi-config-version.cmake")
  set(project_config "${PROJECT_BINARY_DIR}/absscpi-config.cmake")
  set(absscpi_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/absscpi")

  install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/bci"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT devel)

  install(TARGETS absscpi
    EXPORT ${targets_export_name}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT libs)

  write_basic_package_version_file(
    ${version_config}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/absscpi-config.cmake.in
    ${project_config}
    INSTALL_DESTINATION ${absscpi_cmake_dir})

  install(FILES ${project_config} ${version_config}
    DESTINATION ${absscpi_cmake_dir}
    COMPONENT devel)
  install(EXPORT ${targets_export_name} DESTINATION ${absscpi_cmake_dir}
    NAMESPACE bci::
    COMPONENT devel)

  # TODO: generate DEB packages
  set(CPACK_PACKAGE_NAME "absscpi")
  set(CPACK_PACKAGE_VENDOR "Bloomy Controls, Inc.")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
  install(FILES
    ${CPACK_RESOURCE_FILE_LICENSE}
    ${CPACK_RESOURCE_FILE_README}
    DESTINATION .
    COMPONENT Metadata
    EXCLUDE_FROM_ALL)
  set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

  set(CPACK_PACKAGE_INSTALL_DIRECTORY "Bloomy Controls/absscpi")
  set(CPACK_WIX_VERSION 3)
  set(CPACK_WIX_UPGRADE_GUID "DF57E70F-C4C5-42E5-9047-44DC481FE555")
  set(CPACK_WIX_ROOT_FEATURE_TITLE "ABS SCPI Driver")
  set(CPACK_WIX_ROOT_FEATURE_DESCRIPTION
    "ABS SCPI driver C/C++ library and development files")
  set(CPACK_WIX_PROPERTY_ARPCOMMENTS "ABS SCPI Driver C/C++ Library")
  set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${PROJECT_HOMEPAGE_URL}")
  set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY "absscpi")
  set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/img/installer_banner.jpg")
  set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/img/installer_dialog.jpg")

  if(WIN32)
    # this will create a "package" target on Windows so that we don't have to
    # invoke CPack directly
    set(CPACK_GENERATOR "WIX")
  endif()

  include(CPack)

  cpack_add_component(Metadata HIDDEN)
  cpack_add_component(libs REQUIRED
    DISPLAY_NAME "SCPI driver"
    DESCRIPTION "C/C++ SCPI driver library")
  cpack_add_component(devel
    DISPLAY_NAME "Development files"
    DESCRIPTION "Headers and CMake support files for developing software using the libraries")
endif()
