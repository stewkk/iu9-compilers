include(FetchContent)

FetchContent_Declare(
  utfcpp
  GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
  GIT_TAG b26a5f718f4f370af1852a0d5c6ae8fa031ba7d0
)

FetchContent_GetProperties(utfcpp)
if (NOT utfcpp_POPULATED)
  FetchContent_Populate(utfcpp)
  add_subdirectory(${utfcpp_SOURCE_DIR} ${utfcpp_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif ()
