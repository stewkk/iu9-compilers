include(FetchContent)

FetchContent_Declare(
  strong_type
  GIT_REPOSITORY https://github.com/rollbear/strong_type.git
  GIT_TAG 8a2b339474904887cbb9820c48cbe28d8d9af538 # v15
)

FetchContent_GetProperties(strong_type)
if (NOT strong_type_POPULATED)
  FetchContent_Populate(strong_type)
  set(RANGES_RELEASE_BUILD TRUE)
  add_subdirectory(${strong_type_SOURCE_DIR} ${strong_type_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif ()
