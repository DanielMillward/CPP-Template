# Loosely based on: https://github.com/meemknight/cmakeSetup Version 1.1.0

cmake_minimum_required(VERSION 3.20)

# ---- Set names here! ----
set(MY_PROJECT_NAME "myproject")

set(MAIN_EXE_NAME "myexecutable")
set(MAIN_EXE_FILENAME "main.cpp")

set(MAIN_TEST_NAME "mytest")
set(MAIN_TEST_FILENAME "test.cpp")

# ---- Set Project name/language ----
project(${MY_PROJECT_NAME} LANGUAGES CXX)

# ---- Configurable options ----
# Toggle with: cmake -DPRODUCTION_BUILD=ON ..  or a CMake preset
option(PRODUCTION_BUILD "Make this a production build (use relative resources path)" OFF)

# ---- C++ standard ----
# Respect any preset value; otherwise default to C++17
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- MSVC static runtime (Debug gets *Debug; others get non-DLL static) ----
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ---- SDL3 from vendored source (static) ----
set(SDL_SHARED   OFF CACHE BOOL "" FORCE)
set(SDL_STATIC   ON  CACHE BOOL "" FORCE)
set(SDL_TESTS    OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL  OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/sdl3.2.24 EXCLUDE_FROM_ALL)

# ---- GoogleTest from vendored source (static) ----
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/googletest-1.17.0 EXCLUDE_FROM_ALL)
enable_testing()

# ---- App sources (auto-add everything under src/) ----
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/*.c"
     "${CMAKE_SOURCE_DIR}/src/*.cc"
     "${CMAKE_SOURCE_DIR}/src/*.cxx"
     "${CMAKE_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_SOURCE_DIR}/src/*.m"
     "${CMAKE_SOURCE_DIR}/src/*.mm"
)
source_group(TREE "${CMAKE_SOURCE_DIR}/src" FILES ${APP_SOURCES})

# ---- Targets ----
add_executable(${MAIN_EXE_NAME} ${MAIN_EXE_FILENAME} ${APP_SOURCES})
target_include_directories(${MAIN_EXE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${MAIN_EXE_NAME} PRIVATE SDL3::SDL3)

add_executable(${MAIN_TEST_NAME} ${MAIN_TEST_FILENAME} ${APP_SOURCES})
target_include_directories(${MAIN_TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${MAIN_TEST_NAME} PRIVATE SDL3::SDL3 GTest::gtest_main)

# ---- Compile definitions (resource path + build flags) ----
set(_targets ${MAIN_EXE_NAME} ${MAIN_TEST_NAME})

foreach(tgt IN LISTS _targets)
  if(PRODUCTION_BUILD)
    target_compile_definitions(${tgt} PUBLIC
      RESOURCES_PATH="./resources/"
      PRODUCTION_BUILD=1
      DEVELOPMENT_BUILD=0
    )
  else()
    target_compile_definitions(${tgt} PUBLIC
      RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
      PRODUCTION_BUILD=0
      DEVELOPMENT_BUILD=1
    )
  endif()
endforeach()

# ---- IDE niceties & Windows subsystem per build ----
if(MSVC)
  # Set the startup project in Visual Studio
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${CMAKE_PROJECT_NAME}")

  # Hide console window for production builds; keep console in development
  target_link_options(${MAIN_EXE_NAME} PRIVATE
    $<$<BOOL:${PRODUCTION_BUILD}>:/SUBSYSTEM:WINDOWS>
    $<$<NOT:$<BOOL:${PRODUCTION_BUILD}>>:/SUBSYSTEM:CONSOLE>
  )
endif()

if(MINGW)
  # For MinGW, -mwindows hides the console in GUI apps
  target_link_options(${MAIN_EXE_NAME} PRIVATE
    $<$<BOOL:${PRODUCTION_BUILD}>:-mwindows>
  )
endif()
