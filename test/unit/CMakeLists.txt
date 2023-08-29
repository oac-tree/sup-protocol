add_executable(unit-tests)

target_compile_features(unit-tests PRIVATE cxx_std_11)

target_sources(unit-tests
  PRIVATE
  exceptions_tests.cpp
  function_protocol_extract_tests.cpp
  function_protocol_pack_tests.cpp
  function_protocol_tests.cpp
  protocol_result_tests.cpp
  protocol_rpc_client_tests.cpp
  protocol_rpc_server_tests.cpp
  protocol_rpc_tests.cpp
  sup_protocol_di_tests.cpp
  test_protocol.cpp
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
  sup-protocol::sup-protocol
  sup-protocol::sup-protocol-di
)

set_target_properties(unit-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(unit-tests)
