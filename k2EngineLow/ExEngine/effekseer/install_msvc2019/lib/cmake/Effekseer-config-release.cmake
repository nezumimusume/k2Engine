#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Effekseer" for configuration "Release"
set_property(TARGET Effekseer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Effekseer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Effekseer.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Effekseer )
list(APPEND _IMPORT_CHECK_FILES_FOR_Effekseer "${_IMPORT_PREFIX}/lib/Effekseer.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
