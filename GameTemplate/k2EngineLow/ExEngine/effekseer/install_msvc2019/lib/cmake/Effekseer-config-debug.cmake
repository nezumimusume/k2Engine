#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Effekseer" for configuration "Debug"
set_property(TARGET Effekseer APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Effekseer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Effekseerd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Effekseer )
list(APPEND _IMPORT_CHECK_FILES_FOR_Effekseer "${_IMPORT_PREFIX}/lib/Effekseerd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
