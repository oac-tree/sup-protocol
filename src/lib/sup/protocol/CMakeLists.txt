add_library(sup-protocol SHARED)
add_library(sup-protocol::sup-protocol ALIAS sup-protocol) # alias for build-tree usage

set_target_properties(sup-protocol PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  OUTPUT_NAME sup-protocol
)

target_include_directories(sup-protocol PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

target_link_libraries(sup-protocol
  PUBLIC
    sup-dto::sup-dto
  PRIVATE
    sup-utils::sup-codec)

add_subdirectory(base)
add_subdirectory(factory)

# -- Installation --
install(TARGETS sup-protocol EXPORT sup-protocol-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  base64_variable_codec.h
  encoded_variable_t.h
  encoded_variables.h
  exceptions.h
  function_protocol_extract.h
  function_protocol_pack.h
  function_protocol.h
  process_variable.h
  protocol_factory_utils.h
  protocol_factory.h
  protocol_result.h
  protocol_rpc_client.h
  protocol_rpc_server.h
  protocol_rpc.h
  protocol.h
  variable_callback_guard.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/protocol
)
