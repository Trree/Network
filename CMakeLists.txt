cmake_minimum_required(VERSION 3.3)
project (network)
SET(CMAKE_C_COMPILER "gcc") 
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#"${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#"${PROJECT_BINARY_DIR}/share/tcp_server.h"
#)
option (USE_MYMATH "Use tutorial provided math implementation" OFF) 
if (USE_MYMATH)
    include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
    add_subdirectory (MathFunctions)
    set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)


# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
ADD_DEFINITIONS(-std=c99 -g3 -W -Wall -pedantic -D_GNU_SOURCE)
#ADD_DEFINITIONS(-std=c99 -g -W)

aux_source_directory(share DIR_SRCS)
# add the executable
add_executable(client client/tcp_client.c ${DIR_SRCS})
add_executable(sselect server/tcp_select.c ${DIR_SRCS})
add_executable(sfork server/tcp_fork.c   ${DIR_SRCS})
add_executable(spoll server/tcp_poll.c   ${DIR_SRCS})
add_executable(sepoll server/tcp_epoll.c   ${DIR_SRCS})

install (TARGETS client DESTINATION  /home/koal/network/bin)
install (TARGETS sselect DESTINATION  /home/koal/network/bin)
install (TARGETS sfork  DESTINATION  /home/koal/network/bin)
install (TARGETS spoll  DESTINATION  /home/koal/network/bin)
install (TARGETS sepoll  DESTINATION  /home/koal/network/bin)
