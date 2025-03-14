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
target_link_libraries(liblexer PRIVATE strong_type::strong_type)
target_include_directories(liblexer SYSTEM PRIVATE
  "${FETCHCONTENT_BASE_DIR}/immer-src"
  "${FETCHCONTENT_BASE_DIR}/mach7-src/code"
  "${FETCHCONTENT_BASE_DIR}/utfcpp-src/source"
)

add_executable(lab4 main.cpp)
target_compile_features(lab4 PRIVATE cxx_std_20)
set_target_properties(lab4 PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(lab4 PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(lab4 PRIVATE liblexer)
