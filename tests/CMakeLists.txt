find_package(GTest REQUIRED)
include(GoogleTest)

aux_source_directory(./ ABC_TEST_SOURCES)
aux_source_directory(./tl_expected TL_EXPECTED_SRC)
aux_source_directory(./uint128 UINT128_SRC)

add_executable(tests ${ABC_TEST_SOURCES} ${TL_EXPECTED_SRC} ${UINT128_SRC})

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_directories(tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(tests PRIVATE ${ABC_LIBRARY} fmt::fmt-header-only GTest::GTest GTest::Main)

gtest_discover_tests(tests)
