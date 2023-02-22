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

find_package(sup-dto REQUIRED)
target_link_libraries(sup-protocol PUBLIC sup-dto::sup-dto)

target_sources(sup-protocol
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/function_protocol_extract.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function_protocol_pack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function_protocol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_result.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc_client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/protocol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rpc_exceptions.cpp
)

# -- Installation --

install(TARGETS sup-protocol EXPORT sup-protocol-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  function_protocol_extract.h
  function_protocol_pack.h
  function_protocol.h
  protocol_result.h
  protocol_rpc_client.h
  protocol_rpc_server.h
  protocol_rpc.h
  protocol.h
  rpc_exceptions.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/protocol
)
