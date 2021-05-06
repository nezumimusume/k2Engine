#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EffekseerRendererGL" for configuration "Debug"
set_property(TARGET EffekseerRendererGL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(EffekseerRendererGL PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/EffekseerRendererGLd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS EffekseerRendererGL )
list(APPEND _IMPORT_CHECK_FILES_FOR_EffekseerRendererGL "${_IMPORT_PREFIX}/lib/EffekseerRendererGLd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
