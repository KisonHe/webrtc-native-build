cmake_minimum_required(VERSION 3.10.0)

# project(webrtc-native-build NONE)
project(webrtc-native-build CXX)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(
    -D_GLIBCXX_USE_CXX11_ABI=0
)

# Verify build architecture
if (CMAKE_BUILD_ARCHITECTURE MATCHES arm64)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR arm64)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_SYSROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/webrtc_native/webrtc/src/build/linux/debian_bullseye_arm64-sysroot)
    set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
elseif (CMAKE_BUILD_ARCHITECTURE MATCHES arm32)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR armhf)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_SYSROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/webrtc_native/webrtc/src/build/linux/debian_bullseye_arm-sysroot)
    set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
elseif (CMAKE_BUILD_ARCHITECTURE MATCHES win64)
    # Nothing to do
else()
    # Will use native compiler
    if (APPLE)
        set(CMAKE_BUILD_ARCHITECTURE osx64)
    elseif(WIN32)
        set(CMAKE_BUILD_ARCHITECTURE win64)
    else()
        set(CMAKE_BUILD_ARCHITECTURE amd64)
    endif()

    set(CMAKE_SYSROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/webrtc_native/webrtc/src/build/linux/debian_bullseye_amd64-sysroot)
endif()

# Verify build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


message(STATUS "Building ${CMAKE_PROJECT_NAME} with Architecture: ${CMAKE_BUILD_ARCHITECTURE} Build type: ${CMAKE_BUILD_TYPE}")


file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/VERSION VERSION_FROM_FILE)
string(REPLACE "." ";" VERSION_LIST ${VERSION_FROM_FILE})
list(GET VERSION_LIST 0 WEBRTC_NATIVE_BUILD_VERSION_MAJOR)
list(GET VERSION_LIST 1 WEBRTC_NATIVE_BUILD_VERSION_MINOR)
list(GET VERSION_LIST 2 WEBRTC_NATIVE_BUILD_VERSION_PATCH)

SET(WEBRTC_NATIVE_BUILD_VERSION ${WEBRTC_NATIVE_BUILD_VERSION_MAJOR}.${WEBRTC_NATIVE_BUILD_VERSION_MINOR}.${WEBRTC_NATIVE_BUILD_VERSION_PATCH})

message(STATUS "WEBRTC_NATIVE_BUILD_VERSION : ${WEBRTC_NATIVE_BUILD_VERSION}")



if (POLICY CMP0014)
    cmake_policy(SET CMP0014 NEW)
endif (POLICY CMP0014)

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)




set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/dist/${CMAKE_BUILD_TYPE}/${CMAKE_PROJECT_NAME}-${WEBRTC_NATIVE_BUILD_VERSION})
message(STATUS "CMAKE_INSTALL_PREFIX : ${CMAKE_INSTALL_PREFIX}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DRELEASE=1)
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif (MSVC)

function(assign_source_group)
    foreach (_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else ()
            set(_source_rel "${_source}")
        endif ()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach ()
endfunction(assign_source_group)

if (WIN32)
    # set(CMAKE_CXX_FLAGS "-fPIC")
    # add_definitions(-DWEBRTC_POSIX)
    list(APPEND boringssl_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/third_party/boringssl/src/include")
    list(APPEND webrtc_native_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/out/${CMAKE_BUILD_TYPE}/obj/webrtc.lib")
    list(APPEND boringssl_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/out/${CMAKE_BUILD_TYPE}/obj/third_party/boringssl/boringssl.lib")
    list(APPEND libyuv_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/out/${CMAKE_BUILD_TYPE}/obj/third_party/libyuv/libyuv_internal.lib")

    set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-msvc-2019-win64-${CMAKE_BUILD_TYPE}-${WEBRTC_NATIVE_BUILD_VERSION})
    set(CPACK_GENERATOR "ZIP")
    include(CPack)
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "-fPIC")
    add_definitions(-DWEBRTC_POSIX)
    list(APPEND boringssl_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/third_party/boringssl/src/include")
    list(APPEND webrtc_native_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/out/${CMAKE_BUILD_TYPE}/obj/libwebrtc.a" dl X11 expat)
    list(APPEND boringssl_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/out/${CMAKE_BUILD_TYPE}/obj/third_party/boringssl/libboringssl.a")
    list(APPEND libyuv_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/3rdParty/webrtc_native/webrtc/src/out/${CMAKE_BUILD_TYPE}/obj/third_party/libyuv/libyuv_internal.a")
endif (WIN32)


add_executable(
    webrtc_demo 
    # src/video_loopback_main.cc 
    # src/video_loopback.cc

    # src/peerconnection/server/main.cc
    # src/peerconnection/server/peer_channel.cc
    # src/peerconnection/server/data_socket.cc
    # src/peerconnection/server/utils.cc

    # src/peerconnection/client/linux/main.cc
    # src/peerconnection/client/linux/main_wnd.cc
    # src/peerconnection/client/conductor.cc
    # src/peerconnection/client/defaults.cc
    # src/peerconnection/client/defaults.cc
    # src/peerconnection/client/peer_connection_client.cc

    # # /home/kisonhe/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/api/jsep.cc
    # /home/kisonhe/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/test/vcm_capturer.cc
    # /home/kisonhe/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/rtc_base/strings/json.cc
    # # /home/kisonhe/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/pc/jsep_session_description.cc

    src/main.cpp
    src/i420_creator.cpp


    )
include_directories(
    3rdParty/webrtc_native/webrtc/src
    3rdParty/webrtc_native/webrtc/src/third_party/abseil-cpp
    3rdParty/webrtc_native/webrtc/src/third_party/libyuv/include
    3rdParty/webrtc_native/webrtc/src/third_party/googletest/custom
    3rdParty/webrtc_native/webrtc/src/third_party/googletest/src/googlemock/include
    3rdParty/webrtc_native/webrtc/src/third_party/googletest/src/googletest/include
    3rdParty/webrtc_native/webrtc/src/third_party/jsoncpp/source/include
    )


    include_directories(
        third_party/webrtc/linux/include
        third_party/webrtc/linux/include/out/Default_x64/gen
        buildtools/third_party/libc++

        third_party/googletest/custom
        third_party/googletest/src/googlemock/include
        third_party/googletest/src/googletest/include
        /usr/include/glib-2.0
)
include(FindPkgConfig)
find_package(PkgConfig REQUIRED)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
# pkg_check_modules(absl REQUIRED IMPORTED_TARGET absl_base)
# target_link_libraries(
#     webrtc_demo 
#     PkgConfig::glib 
#     PkgConfig::GTK 
#     PkgConfig::absl
#     )
# link_directories(
#     ${absl_LIBRARY_DIRS}
#     )
target_link_libraries(
    webrtc_demo
    # ${absl_LIBRARIES}
    ${GTK_LIBRARIES}
    ${glib_LIBRARIES}
    )
    # message(FATAL_ERROR ${absl_LIBRARIES})
    # message(FATAL_ERROR ${absl_CFLAGS_OTHER})

target_include_directories(webrtc_demo PUBLIC 
    # ${absl_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${glib_INCLUDE_DIRS}
    )
target_compile_options(webrtc_demo PUBLIC 
    # ${absl_CFLAGS_OTHER}
    ${GTK_CFLAGS_OTHER}
    ${glib_CFLAGS_OTHER}
    )

set_target_properties(webrtc_demo PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(3rdParty)
# add_subdirectory(3rdParty/webrtc_native/webrtc/src/third_party/abseil-cpp)
add_subdirectory(3rdParty/webrtc_native/webrtc/src/third_party/jsoncpp/source)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -pthread -lva -lva-drm -lva-x11 -llzma")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=c++0x -nostdinc++")

# find_package(absl REQUIRED)
target_link_libraries(
    webrtc_demo 
    # ${CMAKE_SOURCE_DIR}/3rdParty/webrtc_native/webrtc/src/out/Debug/obj/libwebrtc.a
    # ${CMAKE_SOURCE_DIR}/3rdParty/webrtc_native/webrtc/src/out/Debug/obj/rtc_base/librtc_base.a
    # ${CMAKE_SOURCE_DIR}/3rdParty/webrtc_native/webrtc/src/out/Debug/obj/rtc_base/librtc_event.a
    ${webrtc_native_LIBRARY}
    ${boringssl_LIBRARY}
    ${libyuv_LIBRARY}
    X11
    dl
    # /home/kisonhe/jsoncpp/source/build/lib/libjsoncpp.a
    # jsoncpp
    jsoncpp_lib
    # /home/kisonhe/webrtc-native-build/3rdParty/webrtc_native/webrtc/src/out/Debug/obj/api/libjingle_peerconnection_api/jsep_ice_candidate.o
    )