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
ELSE()
ADD_SUBDIRECTORY(player)
ENDIF()



