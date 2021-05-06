#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Effekseer" for configuration "MinSizeRel"
set_property(TARGET Effekseer APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(Effekseer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/Effekseer.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Effekseer )
list(APPEND _IMPORT_CHECK_FILES_FOR_Effekseer "${_IMPORT_PREFIX}/lib/Effekseer.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
