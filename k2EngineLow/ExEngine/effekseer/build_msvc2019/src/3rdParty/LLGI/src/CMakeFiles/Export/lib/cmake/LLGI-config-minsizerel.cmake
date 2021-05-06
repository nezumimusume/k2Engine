#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LLGI" for configuration "MinSizeRel"
set_property(TARGET LLGI APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(LLGI PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/LLGI.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS LLGI )
list(APPEND _IMPORT_CHECK_FILES_FOR_LLGI "${_IMPORT_PREFIX}/lib/LLGI.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
