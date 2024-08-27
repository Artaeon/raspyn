cmake_minimum_required(VERSION 3.10)
project(raspyn)

set(CMAKE_C_STANDARD 17)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    link_libraries(ws2_32)
endif()

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(include)

add_executable(server src/server.c src/raspyn.c)
target_link_libraries(server ${OPENSSL_LIBRARIES})

add_executable(client src/client.c src/raspyn.c)
target_link_libraries(client ${OPENSSL_LIBRARIES})

add_executable(test_raspyn tests/test_raspyn.c src/raspyn.c)
target_link_libraries(test_raspyn ${OPENSSL_LIBRARIES})