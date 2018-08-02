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


ADD_SUBDIRECTORY(playerlib)
IF (IOS_PLATFORM)
ELSEIF(ANDROID)
#
#  copy FFLv2-lib
#
#execute_process(
# COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/player-android/FFLv2-lib-android
# COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../FFLv2-lib/ ${CMAKE_CURRENT_SOURCE_DIR}/player-android/FFLv2-lib-android/
#)
#  方便调试，直接添加进去了
#
#ADD_SUBDIRECTORY(player-android/FFLv2-lib-android)
ELSE()
ADD_SUBDIRECTORY(player)
ENDIF()



set( CASSDK_DIR ${CMAKE_SOURCE_DIR}/../FFLv2-lib)

if( IS_DIRECTORY ${CASSDK_DIR} )
    # 第二个cassdk.out参数用于指定外部文件夹在输出文件夹中的位置
 add_subdirectory( ${CASSDK_DIR} FFLv2.out )
 
endif()


