#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EffekseerRendererDX9" for configuration "Release"
set_property(TARGET EffekseerRendererDX9 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(EffekseerRendererDX9 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/EffekseerRendererDX9.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS EffekseerRendererDX9 )
list(APPEND _IMPORT_CHECK_FILES_FOR_EffekseerRendererDX9 "${_IMPORT_PREFIX}/lib/EffekseerRendererDX9.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
