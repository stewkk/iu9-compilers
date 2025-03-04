include(FetchRe2)
include(FetchRangeV3)

add_library(liblexer
  lexer.cpp
)
add_library(stewkk::liblexer ALIAS liblexer)
target_compile_features(liblexer PUBLIC cxx_std_20)
set_target_properties(liblexer PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
set_target_properties(liblexer PROPERTIES OUTPUT_NAME stewkk_lexer)
target_include_directories(
  liblexer PUBLIC $<INSTALL_INTERFACE:include>
                    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_options(liblexer PRIVATE ${BASE_COMPILE_FLAGS})
target_link_options(liblexer PRIVATE ${BASE_LINK_FLAGS})
target_link_libraries(liblexer PRIVATE re2::re2 range-v3::range-v3)

add_executable(lab3 main.cpp)
target_compile_features(lab3 PRIVATE cxx_std_20)
set_target_properties(lab3 PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(lab3 PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(lab3 PRIVATE liblexer)
