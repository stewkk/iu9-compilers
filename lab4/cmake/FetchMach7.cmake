include(FetchContent)

FetchContent_Declare(
  mach7
  GIT_REPOSITORY https://github.com/solodon4/Mach7.git
  GIT_TAG 949e0f1fbf5b6c725ad460137c4f08961fc78f52
  SOURCE_SUBDIR "MADE-UP-DIRECTORY"
)

FetchContent_GetProperties(mach7)
if (NOT mach7_POPULATED)
  FetchContent_Populate(mach7)
endif ()
