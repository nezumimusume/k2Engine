# Install script for directory: C:/Users/Takay/Downloads/EffekseerRuntime160/src/EffekseerRendererDX11

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Takay/Downloads/EffekseerRuntime160/install_msvc2019")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/Debug/EffekseerRendererDX11d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/Release/EffekseerRendererDX11.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/MinSizeRel/EffekseerRendererDX11.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/RelWithDebInfo/EffekseerRendererDX11.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/EffekseerRendererDX11" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/src/EffekseerRendererDX11/EffekseerRendererDX11.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/EffekseerRendererDX11-config.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/EffekseerRendererDX11-config.cmake"
         "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/CMakeFiles/Export/lib/cmake/EffekseerRendererDX11-config.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/EffekseerRendererDX11-config-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/EffekseerRendererDX11-config.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/CMakeFiles/Export/lib/cmake/EffekseerRendererDX11-config.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/CMakeFiles/Export/lib/cmake/EffekseerRendererDX11-config-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/CMakeFiles/Export/lib/cmake/EffekseerRendererDX11-config-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/CMakeFiles/Export/lib/cmake/EffekseerRendererDX11-config-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/EffekseerRendererDX11/CMakeFiles/Export/lib/cmake/EffekseerRendererDX11-config-release.cmake")
  endif()
endif()

