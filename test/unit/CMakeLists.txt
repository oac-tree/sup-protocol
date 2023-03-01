add_executable(unit-tests)

target_compile_features(unit-tests PRIVATE cxx_std_11)

target_sources(unit-tests
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/exceptions_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function_protocol_extract_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function_protocol_pack_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function_protocol_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_result_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc_client_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc_server_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_protocol.cpp
)

target_include_directories(unit-tests
  PRIVATE
)

find_package(GTest REQUIRED)

include(GoogleTest)

target_link_libraries(unit-tests
  PRIVATE
    GTest::GTest
    GTest::Main
    sup-protocol
)

set_target_properties(unit-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(unit-tests)
