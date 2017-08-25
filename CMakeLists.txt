cmake_minimum_required(VERSION 3.1)
project(GpdServer)

#set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(PROGNAME GpdServer)
set(SRC_FOLDER ${PROJECT_SOURCE_DIR}/src)
set(PGD_SOURCES ${SRC_FOLDER}/main.cpp
                ${SRC_FOLDER}/TcpServer.cpp
                ${SRC_FOLDER}/TcpConnection.cpp
                ${SRC_FOLDER}/OgdfTest.cpp
)

set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_MULTITHREADED ON)

add_subdirectory(lib/ogdf)

find_package(Boost 1.58.0 REQUIRED COMPONENTS system)
find_package (Threads)

if (APPLE)
    SET(OPENSSL_ROOT_DIR /usr/local/opt/openssl/)
endif (APPLE)
find_package(OpenSSL)

include_directories(${Boost_INCLUDE_DIRS}
                    ${OPENSSL_INCLUDE_DIR}
                    lib/ogdf/include
                    lib/jsonparser/include
                    ${CMAKE_BINARY_DIR}/include
)

add_executable(${PROGNAME} ${PGD_SOURCES})
target_link_libraries(${PROGNAME} ${Boost_LIBRARIES}
                                  ${OPENSSL_LIBRARIES}
                                    OGDF
                                    COIN
                                    ${CMAKE_THREAD_LIBS_INIT}
)
