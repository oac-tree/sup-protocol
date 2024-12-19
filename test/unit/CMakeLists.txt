add_executable(unit-tests)

target_sources(unit-tests
  PRIVATE
  async_invoke_server_tests.cpp
  async_invoke_tests.cpp
  exceptions_tests.cpp
  expiration_timeout_handler_tests.cpp
  function_protocol_extract_tests.cpp
  function_protocol_pack_tests.cpp
  function_protocol_tests.cpp
  process_variable_utils_tests.cpp
  process_variable_tests.cpp
  protocol_factory_utils_tests.cpp
  protocol_result_tests.cpp
  protocol_rpc_client_async_tests.cpp
  protocol_rpc_client_config_tests.cpp
  protocol_rpc_client_server_tests.cpp
  protocol_rpc_client_tests.cpp
  protocol_rpc_server_async_tests.cpp
  protocol_rpc_server_tests.cpp
  protocol_rpc_tests.cpp
  sup_protocol_di_tests.cpp
  test_functor.cpp
  test_process_variable.cpp
  test_protocol.cpp
)

target_include_directories(unit-tests
  PRIVATE
)

target_link_libraries(unit-tests
  PRIVATE
  GTest::GTest
  GTest::Main
  GTest::gmock
  sup-protocol::sup-protocol
  sup-protocol::sup-protocol-di
)

set_target_properties(unit-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})

gtest_discover_tests(unit-tests)
