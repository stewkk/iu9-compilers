include(FetchContent)

FetchContent_Declare(
  range-v3
  GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
  GIT_TAG 7e6f34b1e820fb8321346888ef0558a0ec842b8e
)

FetchContent_GetProperties(range-v3)
if (NOT range-v3_POPULATED)
  FetchContent_Populate(range-v3)
  set(RANGES_RELEASE_BUILD TRUE)
  add_subdirectory(${range-v3_SOURCE_DIR} ${range-v3_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif ()
