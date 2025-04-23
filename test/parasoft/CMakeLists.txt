if(NOT COA_PARASOFT_INTEGRATION)
  return()
endif()

include(COASetupParasoft)

# Add parasoft project for sup-protocol
# This generates the build time information needed by Parasoft for analysis
cpptest_add_executable(sup-protocol-cpptest
  CPPTEST_PROJECT_LOC ${CMAKE_CURRENT_BINARY_DIR}
  CPPTEST_PROJECT_FOLDERS
    sup-protocol=${CMAKE_CURRENT_SOURCE_DIR}/src/lib/sup/protocol
    sup-protocol-di=${CMAKE_CURRENT_SOURCE_DIR}/src/lib/sup/protocol-di
  TARGETS sup-protocol sup-protocol-di
)

get_target_property(SUP_PROTOCOL_INCLUDE_DIRECTORIES sup-protocol INCLUDE_DIRECTORIES)
target_include_directories(sup-protocol-cpptest PUBLIC ${SUP_PROTOCOL_INCLUDE_DIRECTORIES})

target_link_libraries(sup-protocol-cpptest
  PUBLIC
  sup-di::sup-di
  sup-dto::sup-dto
  sup-utils::sup-codec
)
