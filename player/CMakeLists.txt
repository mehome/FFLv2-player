cmake_minimum_required(VERSION 3.4.1)

PROJECT(player)
IF (WIN32)
INCLUDE_DIRECTORIES(
            ./
			../playerlib/
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/
            ../third/SDL2/include/ )
LINK_DIRECTORIES(
            ../third/FFmpeg3.3.5/output_win_dll/bin/
            ../third/SDL2/lib/window/
			../../install/FFLv2-lib/libs/ )
ELSEIF(IOS_PLATFORM)
INCLUDE_DIRECTORIES(
            ./
			../playerlib/
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/
            ../third/SDL2/include/ )


ELSEIF (APPLE)
INCLUDE_DIRECTORIES(
            ./
			../playerlib/
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/
            ../third/SDL2/include/ )
LINK_DIRECTORIES(
            ../third/FFmpeg3.3.5/output_mac_dylib/bin/
            ../third/SDL2/lib/mac/
			../../install/FFLv2-lib/libs/ )
ELSEIF(ANDROID)
INCLUDE_DIRECTORIES(
            ./
			../playerlib/
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/ )
LINK_DIRECTORIES(
            ../third/FFmpeg3.3.5/output_android_static/
			../../install/FFLv2-lib/libs/)

ENDIF ()

SET(LOCAL_SRC_FILES
   main.cpp
   player.cpp)


if (MSVC)
    add_definitions("/wd4819")
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif(MSVC)



SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../bin)

ADD_EXECUTABLE(player  ${LOCAL_SRC_FILES})


IF (WIN32)
TARGET_LINK_LIBRARIES(player playerLib FFL avcodec avdevice avfilter avformat avutil swscale swresample SDL2)
ELSEIF (IOS_PLATFORM)
TARGET_LINK_LIBRARIES(player playerLib FFL avcodec avdevice avfilter avformat avutil swscale swresample SDL2)
ELSEIF (APPLE)
TARGET_LINK_LIBRARIES(player playerLib FFL avcodec avdevice avfilter avformat avutil swscale swresample SDL2)
ELSEIF (ANDROID)
TARGET_LINK_LIBRARIES(player playerLib FFL libffmpeg.so)
ENDIF ()



IF (WIN32)
SET(OUTPUT_BIN ${EXECUTABLE_OUTPUT_PATH}${CMAKE_BUILD_TYPE})
add_custom_command(TARGET player POST_BUILD    
    COMMAND echo "hi " ${CMAKE_BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/../third/FFmpeg3.3.5/output_win_dll/bin/  ${OUTPUT_BIN}/Debug/)
ENDIF()

