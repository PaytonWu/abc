find_package(GTest REQUIRED)
include(GoogleTest)

aux_source_directory(./ ABC_TEST_SOURCES)
aux_source_directory(./tl_expected TL_EXPECTED_SRC)
aux_source_directory(./uint128 UINT128_SRC)
aux_source_directory(./async ASYNC_TEST_SOURCES)

add_executable(tests ${ABC_TEST_SOURCES} ${TL_EXPECTED_SRC} ${UINT128_SRC} ${ASYNC_TEST_SOURCES})

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/stdexec/include)

target_link_directories(tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(tests PRIVATE ${ABC_LIBRARY} fmt::fmt-header-only GTest::GTest GTest::Main)

gtest_discover_tests(tests)
