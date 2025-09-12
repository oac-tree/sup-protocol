set(library_name sup-protocol)

add_library(${library_name} SHARED)
add_library(${library_name}::${library_name} ALIAS ${library_name}) # alias for build-tree usage

set_target_properties(${library_name} PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  EXPORT_NAME ${library_name}
)

target_include_directories(${library_name} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

target_link_libraries(${library_name}
  PUBLIC
    sup-dto::sup-dto
    sup-di::sup-di
  PRIVATE
    sup-utils::sup-codec)

add_subdirectory(base)
add_subdirectory(decorators)
add_subdirectory(factory)

# -- Installation --
install(TARGETS ${library_name} EXPORT ${library_name}-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  base64_variable_codec.h
  encoded_variable_t.h
  encoded_variables.h
  exceptions.h
  function_protocol_extract.h
  function_protocol_pack.h
  function_protocol.h
  log_any_functor_decorator.h
  log_protocol_decorator.h
  process_variable.h
  protocol_factory_utils.h
  protocol_factory.h
  protocol_result.h
  protocol_rpc_client_config.h
  protocol_rpc_client.h
  protocol_rpc_server_config.h
  protocol_rpc_server.h
  protocol_rpc.h
  protocol.h
  registered_names.h
  variable_callback_guard.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/protocol
)
