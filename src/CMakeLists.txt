option(BUILD_CLIENT_ONLY "*** Build client" OFF)
option(BUILD_SERVER_ONLY "*** Build server" OFF)
option(BUILD_ALL "*** Build server & client" OFF)


if (NOT BUILD_CLIENT_ONLY AND NOT BUILD_SERVER_ONLY AND NOT BUILD_ALL)
    message(STATUS "*** Use BUILD_ALL(default)")
    set(BUILD_ALL ON)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "*** Build Release Version")
else()
    message(STATUS "*** Build Debug Version")
endif()


if (BUILD_CLIENT_ONLY OR BUILD_ALL)
    message(STATUS "*** Build Client ONLY")
    add_subdirectory(TcpClient)
endif()

if (BUILD_SERVER_ONLY OR BUILD_ALL)
    message(STATUS "*** Build Server ONLY")
    add_subdirectory(TcpServer)
endif()