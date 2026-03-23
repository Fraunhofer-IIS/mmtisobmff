cmake_minimum_required( VERSION 3.16.0 )
include(FetchContent)

project(mmtisobmff
  VERSION 1.0.4
  DESCRIPTION "Fraunhofer modern ISOBMFF Library"
  HOMEPAGE_URL "https://github.com/Fraunhofer-IIS/mmtisobmff"
  LANGUAGES CXX C
)

FetchContent_Declare(
  ilo
  GIT_REPOSITORY https://github.com/Fraunhofer-IIS/ilo.git
  GIT_TAG        r2.0.2
)

if(mmtisobmff_included)
  return()
endif(mmtisobmff_included)
set(mmtisobmff_included true)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

get_directory_property(parentDir PARENT_DIRECTORY)

# Only enable building binaries by default if project is top-level
if(parentDir)
  set(mmtisobmff_BUILD_BINARIES OFF CACHE BOOL   "Build demo binaries")
else()
  set(mmtisobmff_BUILD_BINARIES ON  CACHE BOOL   "Build demo binaries")
endif()
set(mmtisobmff_BUILD_DOC        OFF CACHE BOOL   "Build doxygen doc")
set(USE_PKGCONFIG_DEPS          OFF CACHE BOOL   "Use pkg-config to find dependencies")

if(USE_PKGCONFIG_DEPS)
  find_package(PkgConfig)
  pkg_check_modules(ILO REQUIRED IMPORTED_TARGET GLOBAL ilo>=2.0)
  add_library(ilo ALIAS PkgConfig::ILO)
else()
  FetchContent_MakeAvailable(ilo)
endif()

include_directories(include src)
add_subdirectory(src)

target_include_directories(mmtisobmff PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
  $<INSTALL_INTERFACE:include>
)

if(mmtisobmff_BUILD_BINARIES)
  add_subdirectory(demo)
endif()

if(${PROJECT_NAME}_BUILD_DOC)
  add_subdirectory(doc)
endif()

# Install
include(GNUInstallDirs)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mmtisobmff.pc.in ${CMAKE_CURRENT_BINARY_DIR}/mmtisobmff.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mmtisobmff.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)


