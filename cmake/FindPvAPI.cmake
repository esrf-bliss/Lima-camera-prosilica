set(PVAPI_INCLUDE_DIRS)
set(PVAPI_LIBRARIES)
set(PVAPI_DEFINITIONS)

find_path(PVAPI_INCLUDE_DIRS "PvApi.h")
find_library(PVAPI_LIBRARIES PvAPI)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PvAPI DEFAULT_MSG
  PVAPI_LIBRARIES
  PVAPI_INCLUDE_DIRS
)
