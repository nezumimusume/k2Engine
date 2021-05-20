#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EffekseerRendererDX12" for configuration "Release"
set_property(TARGET EffekseerRendererDX12 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(EffekseerRendererDX12 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/EffekseerRendererDX12.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS EffekseerRendererDX12 )
list(APPEND _IMPORT_CHECK_FILES_FOR_EffekseerRendererDX12 "${_IMPORT_PREFIX}/lib/EffekseerRendererDX12.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
