cmake_minimum_required(VERSION 3.1)
project(BoostServer)

set(PROGNAME BoostServer)
set(SRC_FOLDER ${PROJECT_SOURCE_DIR}/src)
set(PGD_SOURCES ${SRC_FOLDER}/main.cpp
                ${SRC_FOLDER}/TcpServer.cpp
                ${SRC_FOLDER}/TcpConnection.cpp
)

set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_MULTITHREADED ON)

#find_package(Threads)
find_package(Boost 1.58.0 REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(${PROGNAME} ${PGD_SOURCES})
#target_link_libraries(Cpp11Threads ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROGNAME} ${Boost_LIBRARIES})