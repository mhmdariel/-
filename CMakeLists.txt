# CMakeLists.txt for the PLC API
cmake_minimum_required(VERSION 3.14)
project(plc_repository_api VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# JsonCpp (assuming bundled or find package)
find_package(jsoncpp REQUIRED)

# Include directories
include_directories(${CURL_INCLUDE_DIRS}
                    ${OPENSSL_INCLUDE_DIR}
                    ${JSONCPP_INCLUDE_DIRS})

# Library
add_library(plc_repository_api SHARED
    src/plc_repository_api.cpp
)

target_link_libraries(plc_repository_api
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    Threads::Threads
)

# Install
install(TARGETS plc_repository_api
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES include/plc_repository_api.h
    DESTINATION include/autodesk/plc
)
