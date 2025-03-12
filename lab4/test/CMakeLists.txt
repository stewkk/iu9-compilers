include(FetchGTest)
include(FetchImmer)
include(FetchMach7)
include(FetchUtfcpp)
include(GoogleTest)

add_executable(unittests)
target_sources(unittests PRIVATE
  ${PROJECT_SOURCE_DIR}/src/stewkk/lexer/lexer_test.cpp
)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(unittests PRIVATE stewkk::liblexer gmock_main)
target_include_directories(unittests SYSTEM PRIVATE
  "${FETCHCONTENT_BASE_DIR}/immer-src"
  "${FETCHCONTENT_BASE_DIR}/mach7-src/code"
  "${FETCHCONTENT_BASE_DIR}/utfcpp-src/source"
)
gtest_discover_tests(unittests)
