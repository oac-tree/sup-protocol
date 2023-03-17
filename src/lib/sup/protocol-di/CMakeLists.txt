add_library(sup-protocol-di SHARED)
add_library(sup-protocol::sup-protocol-di ALIAS sup-protocol) # alias for build-tree usage

target_compile_features(sup-protocol-di PRIVATE cxx_std_11)

set_target_properties(sup-protocol-di PROPERTIES
  VERSION ${LIBVERSION}
  SOVERSION ${LIBSOVERSION}
  OUTPUT_NAME sup-protocol-di
)

target_include_directories(sup-protocol-di PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
)

find_package(sup-di REQUIRED)

target_link_libraries(sup-protocol-di
  PRIVATE
    sup-protocol::sup-protocol
    sup-di::sup-di
)

target_sources(sup-protocol-di
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/protocol_rpc.cpp
)

# -- Installation --

install(TARGETS sup-protocol-di EXPORT sup-protocol-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES
  registered_names.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sup/protocol-di
)
