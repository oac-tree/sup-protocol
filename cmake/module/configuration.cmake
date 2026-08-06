# -----------------------------------------------------------------------------
# Common project configuration
# -----------------------------------------------------------------------------

include(COACommonConfig)
include(COASetupCodacEnvironment)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(sup-dto REQUIRED)
find_package(sup-utils REQUIRED)
find_package(sup-di REQUIRED)
