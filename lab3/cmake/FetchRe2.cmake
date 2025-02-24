include(FetchContent)

message(CHECK_START "fetching absl")
set(ABSL_ENABLE_INSTALL ON)
set(ABSL_USE_SYSTEM_INCLUDES ON)
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_BUILD_TESTING OFF)
FetchContent_Declare(
  absl
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 9ac7062b1860d895fb5a8cbf58c3e9ef8f674b5f
  GIT_SHALLOW TRUE
)

FetchContent_GetProperties(absl)
if (NOT absl_POPULATED)
  FetchContent_Populate(absl)
  add_subdirectory(${absl_SOURCE_DIR} ${absl_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif ()
message(CHECK_PASS "fetched")

message(CHECK_START "fetching re2")
FetchContent_Declare(
  re2
  GIT_REPOSITORY https://github.com/google/re2.git
  GIT_TAG 6dcd83d60f7944926bfd308cc13979fc53dd69ca
  GIT_SHALLOW TRUE
)

FetchContent_GetProperties(re2)
if (NOT re2_POPULATED)
  FetchContent_Populate(re2)
  add_subdirectory(${re2_SOURCE_DIR} ${re2_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif ()
message(CHECK_PASS "fetched")
