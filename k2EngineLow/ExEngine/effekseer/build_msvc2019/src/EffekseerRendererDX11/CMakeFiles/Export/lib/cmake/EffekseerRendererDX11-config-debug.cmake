#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EffekseerRendererDX11" for configuration "Debug"
set_property(TARGET EffekseerRendererDX11 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(EffekseerRendererDX11 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/EffekseerRendererDX11d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS EffekseerRendererDX11 )
list(APPEND _IMPORT_CHECK_FILES_FOR_EffekseerRendererDX11 "${_IMPORT_PREFIX}/lib/EffekseerRendererDX11d.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
