cmake_minimum_required(VERSION 3.4.1)

IF (WIN32)
    MESSAGE(STATUS "Now is windows")  
ELSEIF (IOS_PLATFORM)
    MESSAGE(STATUS "Now is ios systens.")
ELSEIF (APPLE)
    MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (ANDROID)
    MESSAGE(STATUS "Now is Android os.")  
ELSEIF (UNIX)  
    MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")  
ENDIF () 


PROJECT(playerLib)

IF (WIN32)
INCLUDE_DIRECTORIES(
            ./
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/
            ../third/SDL2/include/ )
LINK_DIRECTORIES(
            ../third/FFmpeg3.3.5/output_win_dll/bin/
            ../third/SDL2/lib/window/ )
ELSEIF(IOS_PLATFORM)
INCLUDE_DIRECTORIES(
            ./
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/
            ../third/SDL2/include/ )


ELSEIF (APPLE)
INCLUDE_DIRECTORIES(
            ./
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/
            ../third/SDL2/include/ )
LINK_DIRECTORIES(
            ../third/FFmpeg3.3.5/output_mac_dylib/bin/
            ../third/SDL2/lib/mac/ )
ELSEIF(ANDROID)
INCLUDE_DIRECTORIES(
            ./
            ../../install/FFLv2-lib/include/
            ../third/FFmpeg3.3.5/include/ )
LINK_DIRECTORIES(
            ../third/FFmpeg3.3.5/output_android_static/)

ENDIF ()

SET(LOCAL_SRC_FILES   
   FFMpeg.cpp;FFMpeg.hpp
   DeviceFactory.cpp;DeviceFactory.hpp
   DeviceManager.cpp;DeviceManager.hpp
   DefaultDeviceManager.cpp;DefaultDeviceManager.hpp
   
      
   PlayerCore.cpp;PlayerCore.hpp
   PlayerEvent.hpp;PlayerEvent.cpp
   PlayerConstant.hpp
   Player.hpp;Player.cpp
   PlayerTrack.hpp;PlayerTrack.cpp
   PlayerInterface.hpp
   Statistic.hpp;Statistic.cpp         
   
   NodeBase.cpp;NodeBase.hpp
   FFMpegDecoder.hpp;FFMpegDecoder.cpp
   FFMpegVideoDecoder.hpp;FFMpegVideoDecoder.cpp
   FFMpegAudioDecoder.hpp;FFMpegAudioDecoder.cpp   
   Decoder.hpp;Decoder.cpp
   Composer.hpp;Composer.cpp  
   AudioUtils.hpp;AudioUtils.cpp 
   AudioFormat.hpp;AudioFormat.cpp
   AudioDevice.hpp;AudioDevice.cpp
   AudioDataCache.hpp;AudioDataCache.cpp
   AudioStream.hpp;AudioStream.cpp
   AudioSample.hpp;AudioSample.cpp   
   AudioRender.hpp;AudioRender.cpp
   AudioResample.hpp;AudioResample.cpp
   AudioComposer.hpp;AudioComposer.cpp
   VideoFormat.hpp;VideoFormat.cpp
   VideoComposer.hpp;VideoComposer.cpp
   VideoUtils.hpp;VideoUtils.cpp 
   VideoSurface.hpp;VideoSurface.cpp      
   VideoDevice.hpp;VideoDevice.cpp
   VideoStream.hpp;VideoStream.cpp
   VideoTexture.hpp;VideoTexture.cpp   
   VideoScale.hpp;VideoScale.cpp   
   VideoRender.hpp;VideoRender.cpp   
   Render.hpp;Render.cpp      
   FFMpegStream.hpp;FFMpegStream.cpp
   MessageBase.hpp;MessageBase.cpp
   MessageFFMpegPacket.hpp;MessageFFMpegPacket.cpp
   MessageFFMpegFrame.hpp;MessageFFMpegFrame.cpp
   TimestampExtrapolator.hpp;TimestampExtrapolator.cpp       
   TimestampUtils.hpp;TimestampUtils.cpp

   SyncClock.hpp;SyncClock.cpp
   SyncUtils.hpp;SyncUtils.cpp
      
   reader/Stream.hpp;reader/Stream.cpp
   reader/ReaderStreamManager.hpp;reader/ReaderStreamManager.cpp
   reader/ReaderFactory.hpp;reader/ReaderFactory.cpp
   reader/ReaderBase.hpp;reader/ReaderBase.cpp
   reader/FFmpegReader.hpp;reader/FFmpegReader.cpp
   )

IF (WIN32)
SET(LOCAL_PLATFORM_SRC_FILES
     SDL2Module.hpp;SDL2Module.cpp
     SDL2AudioDevice.hpp;SDL2AudioDevice.cpp
     SDL2VideoDevice.hpp;SDL2VideoDevice.cpp
	 SDL2MessageTexture.hpp;SDL2MessageTexture.cpp )
ELSEIF (APPLE)
SET(LOCAL_PLATFORM_SRC_FILES
    SDL2Module.hpp;SDL2Module.cpp
    SDL2AudioDevice.hpp;SDL2AudioDevice.cpp
    SDL2VideoDevice.hpp;SDL2VideoDevice.cpp
    SDL2MessageTexture.hpp;SDL2MessageTexture.cpp)
ELSEIF(ANDROID)
SET(LOCAL_PLATFORM_SRC_FILES
   android/render/MemcopyRender.cpp;android/render/MemcopyRender.hpp
   android/render/RenderInterface.hpp
   android/render/RenderManager.cpp;android/render/RenderManager.hpp
   android/render/OpenglesRender.cpp;android/render/OpenglesRender.hpp


   android/AndroidVideoDevice.hpp;android/AndroidVideoDevice.cpp
   android/AndroidAudioDevice.hpp;android/AndroidAudioDevice.cpp
   android/AndroidModule.hpp;android/AndroidModule.cpp
   android/jnihelper/JavaClass.hpp;android/jnihelper/JavaClass.cpp
   android/jnihelper/JavaObject.hpp;android/jnihelper/JavaObject.cpp
   android/jnihelper/JavaThread.hpp;android/jnihelper/JavaThread.cpp
   
   android/NativeFFLPlayer.cpp;android/NativeFFLPlayer.hpp
   android/JavaFFLPlayer.cpp;android/JavaFFLPlayer.hpp
   android/JavaAudiotrack.cpp;android/JavaAudiotrack.hpp
   
   android/opengles2/SDL_shaders_gles2.c;android/opengles2/SDL_shaders_gles2.h
   android/opengles2/SDL_render_gles2.hpp;android/opengles2/SDL_render_gles2.cpp
   android/egl/egl.hpp;android/egl/egl.cpp
   )
 ADD_SUBDIRECTORY(./android/jni)
ELSE()
SET(LOCAL_PLATFORM_SRC_FILES   "")
ENDIF ()
  
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

add_library( playerLib STATIC "${LOCAL_SRC_FILES};${LOCAL_PLATFORM_SRC_FILES}" )
IF (ANDROID)
find_library(log-lib  log )
target_link_libraries( playerLib  android ${log-lib} libffmpeg.so FFL  EGL GLESv2)
ELSE()
ENDIF()

IF (WIN32)
INSTALL(TARGETS playerLib RUNTIME DESTINATION  ${PROJECT_SOURCE_DIR}/../../ide/mfc2015Player/bin
        LIBRARY DESTINATION  ${PROJECT_SOURCE_DIR}/../../ide/mfc2015Player/libs
        ARCHIVE DESTINATION  ${PROJECT_SOURCE_DIR}/../../ide/mfc2015Player/libs)
ENDIF()



