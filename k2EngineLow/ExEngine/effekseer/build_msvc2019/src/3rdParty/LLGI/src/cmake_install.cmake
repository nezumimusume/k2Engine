# Install script for directory: C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/Debug/LLGId.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/Release/LLGI.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/MinSizeRel/LLGI.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/RelWithDebInfo/LLGI.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/LLGI" TYPE FILE FILES
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.Base.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.CommandList.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.Compiler.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.ConstantBuffer.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.Graphics.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.IndexBuffer.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.PipelineState.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.Platform.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.Shader.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.Texture.h"
    "C:/Users/Takay/Downloads/EffekseerRuntime160/src/3rdParty/LLGI/src/LLGI.VertexBuffer.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LLGI-config.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LLGI-config.cmake"
         "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/CMakeFiles/Export/lib/cmake/LLGI-config.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LLGI-config-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LLGI-config.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/CMakeFiles/Export/lib/cmake/LLGI-config.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/CMakeFiles/Export/lib/cmake/LLGI-config-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/CMakeFiles/Export/lib/cmake/LLGI-config-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/CMakeFiles/Export/lib/cmake/LLGI-config-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "C:/Users/Takay/Downloads/EffekseerRuntime160/build_msvc2019/src/3rdParty/LLGI/src/CMakeFiles/Export/lib/cmake/LLGI-config-release.cmake")
  endif()
endif()

