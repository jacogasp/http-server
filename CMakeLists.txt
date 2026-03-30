cmake_minimum_required(VERSION 3.20)

# Source - https://stackoverflow.com/a/79147222
# Posted by usr1234567, modified by community. See post 'Timeline' for change history
# Retrieved 2026-03-30, License - CC BY-SA 4.0
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

if (NOT DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR
          "You must set VCPKG_ROOT env variable or pass CMAKE_TOOLCHAIN_FILE")
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(http-server)

include(CTest)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

# Set standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
find_package(Boost REQUIRED COMPONENTS asio beast)

# Targets
add_library(libhttpserver OBJECT
  src/http_server.cpp
)

target_link_libraries(libhttpserver PUBLIC Boost::asio Boost::beast)

add_executable(http-server
  src/main.cpp
  src/arguments.cpp
)

target_link_libraries(http-server PRIVATE libhttpserver)

# Tests
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
