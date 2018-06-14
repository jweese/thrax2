add_executable(all_tests
  alignment_test.cpp
  label_test.cpp
  phrase_test.cpp
  rule_test.cpp
  sentence_test.cpp
)
target_include_directories(all_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(all_tests PRIVATE gtest gtest_main thrax2)
add_subdirectory(gtest)
gtest_discover_tests(all_tests)

add_executable(hiero_console hiero_console.cpp)
target_include_directories(hiero_console PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(hiero_console PRIVATE thrax2)
