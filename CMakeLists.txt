cmake_minimum_required(VERSION 3.16)
project(p2p-messenger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


string(RANDOM LENGTH 64 ALPHABET "0123456789abcdef" RANDOM_KEY_HEX)
string(REGEX REPLACE "(..)" "0x\\1, " KEY_BYTES "${RANDOM_KEY_HEX}")

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CryptoKey.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/CryptoKey.h"
    @ONLY
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(OPENDHT REQUIRED opendht)
pkg_check_modules(ZLIB REQUIRED zlib)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
pkg_check_modules(OPENSSL REQUIRED openssl)

add_executable(p2p-messenger
    src/main.cpp
    src/LoginDialog.cpp
    src/MainWindow.cpp
    src/ChatWidget.cpp
    src/ContactListWidget.cpp
    src/DhtChatManager.cpp
    src/FileCompressor.cpp
    src/CryptoUtils.cpp
)

qt6_add_resources(p2p-messenger "app_resources"
    PREFIX "/"
    BASE resources
    FILES resources/style.qss
)

target_include_directories(p2p-messenger PRIVATE
    src
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${OPENDHT_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
)

target_link_directories(p2p-messenger PRIVATE
    ${OPENDHT_LIBRARY_DIRS}
    ${JSONCPP_LIBRARY_DIRS}
    ${OPENSSL_LIBRARY_DIRS}
)

target_link_libraries(p2p-messenger PRIVATE
    Qt6::Widgets
    ${OPENDHT_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    Threads::Threads
)