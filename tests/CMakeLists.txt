find_package(GTest REQUIRED)
include(GoogleTest)

if (BUILD_SHARED_LIBS)
    add_definitions(-DABC_DLL)
endif()

aux_source_directory(./ ABC_TEST_SOURCES)
aux_source_directory(./tl_expected TL_EXPECTED_SRC)

add_executable(tests ${ABC_TEST_SOURCES} ${TL_EXPECTED_SRC})
add_dependencies(tests ${ABC_LIBRARY})

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_directories(tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(tests PRIVATE ${ABC_LIBRARY} fmt::fmt-header-only GTest::GTest GTest::Main)

gtest_discover_tests(tests)
