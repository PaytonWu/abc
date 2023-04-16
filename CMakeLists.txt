# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.22)

if (NOT DEFINED ENV{VCPKG_HOME})
    message(FATAL_ERROR "VCPKG_HOME enviornment variable not defined")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")

project(
    ABC
    VERSION 0.1.0
    DESCRIPTION "C++ based utility library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    endif()

    # options for Debug
    add_compile_options($<$<CONFIG:Debug>:-fno-omit-frame-pointer>)

    # options for Release
    add_compile_options($<$<CONFIG:Release>:-ffunction-sections>)
    add_compile_options($<$<CONFIG:Release>:-fdata-sections>)

    # options for RelWithDebInfo
    add_compile_options($<$<CONFIG:Release>:-ffunction-sections>)
    add_compile_options($<$<CONFIG:Release>:-fdata-sections>)

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    # common compile options
    add_compile_options(
        /Zc:__cplusplus       # https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
        /Zc:externConstexpr   # https://docs.microsoft.com/en-us/cpp/build/reference/zc-externconstexpr
        /Zc:referenceBinding  # https://docs.microsoft.com/en-us/cpp/build/reference/zc-referencebinding-enforce-reference-binding-rules
        /Zc:inline            # https://docs.microsoft.com/en-us/cpp/build/reference/zc-inline-remove-unreferenced-comdat
        /Zc:rvalueCast        # https://docs.microsoft.com/en-us/cpp/build/reference/zc-rvaluecast-enforce-type-conversion-rules
        /Zc:strictStrings     # https://docs.microsoft.com/en-us/cpp/build/reference/zc-strictstrings-disable-string-literal-type-conversion
        /Zc:ternary           # https://docs.microsoft.com/en-us/cpp/build/reference/zc-ternary
        /Zc:throwingNew       # https://docs.microsoft.com/en-us/cpp/build/reference/zc-throwingnew-assume-operator-new-throws
        /Zc:char8_t
        /Zc:hiddenFriend
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

    if (CMAKE_CXX_FLAGS MATCHES "/W[0-3]")
        string(REGEX REPLACE "/W[0-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        add_compile_options(/W4)
    endif()

    add_definitions(
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -DNOMINMAX
        -D_WIN32_WINNT=0x0600 # Vista and above
        -D_WINDOWS
        -D_UNICODE
        -DUNICODE
        -DWIN32_LEAN_AND_MEAN)

    # options for Release & RelWithDebInfo
    # /O2 equivalent to /Og /Oi /Ot /Oy /Ob2 /Gs /GF /Gy
    # https://docs.microsoft.com/en-us/cpp/build/reference/o1-o2-minimize-size-maximize-speed
    add_compile_options($<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:/O2>)
else()
    message(FATAL_ERROR "Not supported C++ Compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUG _DEBUG>)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>:NDEBUG>)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(msgpack CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

find_package(xxHash CONFIG REQUIRED)

include_directories(include)
aux_source_directory(src SRCS)
add_library(${ABC_LIBRARY} ${SRCS})
target_link_libraries(${ABC_LIBRARY} PRIVATE fmt::fmt-header-only)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${ABC_LIBRARY} PRIVATE ABC_DLL ABC_EXPORT)
endif()

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
