cmake_minimum_required(VERSION 3.6)

set(APP_NAME demo_memory)
project(${APP_NAME})

set(CONFIGURATION_SUFFIX "$<LOWER_CASE:$<CONFIGURATION>>")

# Only win32 is supported by cocos2d currently
if(CMAKE_GENERATOR_PLATFORM)
	set(PLATFORM_SUFFIX "${CMAKE_GENERATOR_PLATFORM}")
else()
	set(PLATFORM_SUFFIX "win32")
endif()

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE) # to test demo_memory into root project
#    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../etc-bin/cocos2dx-4.0)
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../etc-bin/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     src/AppDelegate.cpp
     src/ListController.cpp
     src/View.cpp
     ../src/BaseView.cpp
     ../src/Game.cpp
     ../src/LoadBalancingListener.cpp
     ../../shared/src/Console.cpp
     )
list(APPEND GAME_HEADER
     inc/AppDelegate.h
     inc/ListController.h
     inc/View.h
     inc/ViewConstants.h
     ../src/BaseView.h
     ../src/DemoConstants.h
     ../src/Game.h
     ../src/LoadBalancingListener.h
     ../../shared/inc/Console.h
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        set(CMAKE_EXE_LINKER_FLAGS
            "-framework Security -framework CFNetwork"
        )
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(CMAKE_EXE_LINKER_FLAGS
            "-framework Security"
        )
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )

if(ANDROID)
    link_directories(
                       ../../../LoadBalancing-cpp/lib/loadbalancing-cpp-prebuilt
                       ../../../Photon-cpp/lib/photon-cpp-prebuilt
                       ../../../Common-cpp/lib/common-cpp-prebuilt
                       ../../../3rdparty/lib/android
    )
elseif(LINUX)
elseif(WINDOWS)
    link_directories(
                       ../../../LoadBalancing-cpp
                       ../../../Photon-cpp
                       ../../../Common-cpp
                       ../../../LoadBalancing-cpp/lib
                       ../../../Photon-cpp/lib
                       ../../../Common-cpp/lib
    )
elseif(APPLE)
    link_directories(
                       ../../../LoadBalancing-cpp/lib
                       ../../../Photon-cpp/lib
                       ../../../Common-cpp/lib
                       ../../../3rdparty/lib/apple
    )
    if(IOS)
    elseif(MACOSX)
    endif()
endif()

if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)

target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
        PRIVATE inc
        PRIVATE ../../shared/inc
        PRIVATE ../../..
)

if(ANDROID)
    add_definitions(-D_EG_ANDROID_PLATFORM)
    target_link_libraries(${APP_NAME}
                       libloadbalancing-cpp-static_${CONFIGURATION_SUFFIX}_android_${ANDROID_ABI}_libc++.a
                       libphoton-cpp-static_${CONFIGURATION_SUFFIX}_android_${ANDROID_ABI}_libc++.a
                       libcommon-cpp-static_${CONFIGURATION_SUFFIX}_android_${ANDROID_ABI}_libc++.a
                       libwebsockets_${CONFIGURATION_SUFFIX}_android_${ANDROID_ABI}_libc++.a
                       libssl_${CONFIGURATION_SUFFIX}_android_${ANDROID_ABI}_libc++.a
                       libcrypto_${CONFIGURATION_SUFFIX}_android_${ANDROID_ABI}_libc++.a
    )
elseif(LINUX)
    add_definitions(-D_EG_LINUX_PLATFORM)
elseif(WINDOWS)
    add_definitions(-D_EG_WINDOWS_PLATFORM)
    target_link_libraries(${APP_NAME}
                       LoadBalancing-cpp_vc15_${CONFIGURATION_SUFFIX}_windows_md_Win32.lib
                       Photon-cpp_vc15_${CONFIGURATION_SUFFIX}_windows_md_Win32.lib
                       Common-cpp_vc15_${CONFIGURATION_SUFFIX}_windows_md_Win32.lib
    )
elseif(APPLE)
    if(IOS)
        add_definitions(-D_EG_IPHONE_PLATFORM)
        target_link_libraries(${APP_NAME}
                       libLoadBalancing-cpp_${CONFIGURATION_SUFFIX}_${CMAKE_OSX_SYSROOT}.a
                       libPhoton-cpp_${CONFIGURATION_SUFFIX}_${CMAKE_OSX_SYSROOT}.a
                       libCommon-cpp_${CONFIGURATION_SUFFIX}_${CMAKE_OSX_SYSROOT}.a
                       libcrypto_${CONFIGURATION_SUFFIX}_${CMAKE_OSX_SYSROOT}.a
        )
    elseif(MACOSX)
        add_definitions(-D_EG_IMAC_PLATFORM)
        target_link_libraries(${APP_NAME}
                       libLoadBalancing-cpp_${CONFIGURATION_SUFFIX}_macosx.a
                       libPhoton-cpp_${CONFIGURATION_SUFFIX}_macosx.a
                       libCommon-cpp_${CONFIGURATION_SUFFIX}_macosx.a
                       libcrypto_${CONFIGURATION_SUFFIX}_macosx.a
        )
    endif()
endif()

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")

    if(MACOSX)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY[sdk=iphoneos*] "iPhone Developer")
		# Set this property to the ID of the team used for signing
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "XSGH3D2939")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
