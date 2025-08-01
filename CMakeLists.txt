# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.22)

# if (NOT DEFINED ENV{VCPKG_HOME})
#     message(FATAL_ERROR "VCPKG_HOME enviornment variable not defined")
# endif()

# set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")

project(
    ABC
    VERSION 0.1.1
    DESCRIPTION "C++ based utility library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "C++ Compiler not found")
endif()

set(ABC_LIBRARY abc)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    # set common options for Clang or GNUCXX
    add_compile_options(
        $<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-fexceptions>
        $<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:-frtti>
    )

    # common compiling options
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -fno-strict-aliasing
        -fthreadsafe-statics
        -pthread
        -fstack-protector-strong
        -fno-short-enums
    )

    add_definitions(-D_GNU_SOURCE)

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        add_compile_options(-Wabstract-final-class -Wshift-overflow -Wint-to-void-pointer-cast)
    endif()

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        add_compile_options(-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override)
        add_compile_definitions($<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>)
        add_compile_definitions($<$<CONFIG:Debug>:_GLIBCXX_DEBUG_PEDANTIC>)
    endif()

    # options for Debug
    add_compile_options($<$<CONFIG:Debug>:-fno-omit-frame-pointer>)
    # add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
    # add_compile_options($<$<CONFIG:Debug>:-static-libasan>)

    # options for Release
    add_compile_options($<$<CONFIG:Release>:-ffunction-sections>)
    add_compile_options($<$<CONFIG:Release>:-fdata-sections>)

    # options for RelWithDebInfo
    add_compile_options($<$<CONFIG:RelWithDebInfo>:-ffunction-sections>)
    add_compile_options($<$<CONFIG:RelWithDebInfo>:-fdata-sections>)

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    # common compile options
    add_compile_options(
        /Zc:__cplusplus       # https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
        /Zc:__STDC__
        /Zc:checkGwOdr
        /Zc:enumTypes
        /Zc:externConstexpr   # https://docs.microsoft.com/en-us/cpp/build/reference/zc-externconstexpr
        /Zc:inline            # https://docs.microsoft.com/en-us/cpp/build/reference/zc-inline-remove-unreferenced-comdat
        /Zc:preprocessor
        /Zc:ternary           # https://docs.microsoft.com/en-us/cpp/build/reference/zc-ternary
        /Zc:throwingNew       # https://docs.microsoft.com/en-us/cpp/build/reference/zc-throwingnew-assume-operator-new-throws
        /permissive-          # https://blogs.msdn.microsoft.com/vcblog/2016/11/16/permissive-switch/
                              # https://blogs.msdn.microsoft.com/vcblog/2017/09/11/two-phase-name-lookup-support-comes-to-msvc/
                              # https://docs.microsoft.com/en-us/cpp/build/reference/permissive-standards-conformance
        /volatile:iso         # https://docs.microsoft.com/en-us/cpp/build/reference/volatile-volatile-keyword-interpretation
        /GS
        /Zi
        /EHsc
        /fp:precise
        /errorReport:prompt
        /diagnostics:caret    # https://docs.microsoft.com/en-us/cpp/build/reference/diagnostics-compiler-diagnostic-options
        /utf-8                # https://docs.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8
        /WL                   # https://docs.microsoft.com/en-us/cpp/build/reference/wl-enable-one-line-diagnostics
    )

    # options for Debug
    add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)

    if (CMAKE_CXX_FLAGS MATCHES "/W[0-3]")
        string(REGEX REPLACE "/W[0-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        add_compile_options(/W4)
    endif()
else()
    message(FATAL_ERROR "Not supported C++ Compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WIN32_WINNT=0x0600 -DWIN32_LEAN_AND_MEAN)
endif()

find_package(Threads REQUIRED)
#find_package(Boost REQUIRED)
find_package(fmt CONFIG REQUIRED)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    find_package(asio CONFIG REQUIRED)
    find_package(range-v3 CONFIG REQUIRED)
endif()

include_directories(include)
include_directories(third_party/stdexec/include)

aux_source_directory(src SRCS)
add_library(${ABC_LIBRARY} ${SRCS})
target_link_libraries(${ABC_LIBRARY} PRIVATE fmt::fmt-header-only)

if (WITH_TESTS)
    add_compile_options(-Wno-empty-body -Wno-unused-parameter)
    enable_testing()
    add_subdirectory(tests)
endif()
