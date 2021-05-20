#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LLGI" for configuration "Debug"
set_property(TARGET LLGI APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LLGI PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/LLGId.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLGI )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLGI "${_IMPORT_PREFIX}/lib/LLGId.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
