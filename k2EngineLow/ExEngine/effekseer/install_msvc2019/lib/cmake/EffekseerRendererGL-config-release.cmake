#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EffekseerRendererGL" for configuration "Release"
set_property(TARGET EffekseerRendererGL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(EffekseerRendererGL PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/EffekseerRendererGL.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS EffekseerRendererGL )
list(APPEND _IMPORT_CHECK_FILES_FOR_EffekseerRendererGL "${_IMPORT_PREFIX}/lib/EffekseerRendererGL.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
