add_library(sup-protocol SHARED)
add_library(sup-protocol::sup-protocol ALIAS sup-protocol) # alias for build-tree usage

target_compile_features(sup-protocol PRIVATE cxx_std_11)

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

target_sources(sup-protocol
  PRIVATE
  exceptions.cpp
  function_protocol_extract.cpp
  function_protocol_pack.cpp
  function_protocol.cpp
  protocol_encodings.cpp
  protocol_factory_utils.cpp
  protocol_factory.cpp
  protocol_result.cpp
  protocol_rpc_client.cpp
  protocol_rpc_server.cpp
  protocol_rpc.cpp
  protocol.cpp
)

# -- Installation --
install(TARGETS sup-protocol EXPORT sup-protocol-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  exceptions.h
  function_protocol_extract.h
  function_protocol_pack.h
  function_protocol.h
  protocol_factory_utils.h
  protocol_factory.h
  protocol_result.h
  protocol_rpc_client.h
  protocol_rpc_server.h
  protocol_rpc.h
  protocol.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/protocol
)
