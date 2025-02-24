include(FetchContent)

FetchContent_Declare(
  immer
  GIT_REPOSITORY https://github.com/arximboldi/immer.git
  GIT_TAG df6ef46d97e1fe81f397015b9aeb32505cef653b
  SOURCE_SUBDIR "MADE-UP-DIRECTORY"
)

FetchContent_GetProperties(immer)
if (NOT immer_POPULATED)
  FetchContent_Populate(immer)
endif ()
