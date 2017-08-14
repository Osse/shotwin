cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(ShotwinTests)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc and uic automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Widgets Sql)
find_package(Exiv2 REQUIRED)

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)


set(SOURCES
    main.cpp
    ../shotwin.cpp
    ../models/eventitem.cpp
    ../models/eventtreeitem.cpp
    ../models/eventtreemodel.cpp
    ../models/eventfilteredphotomodel.cpp
    ../models/hidephotosproxymodel.cpp
    ../models/monthitem.cpp
    ../models/photoitem.cpp
    ../models/photomodel.cpp
    ../models/rootitem.cpp
    ../models/yearitem.cpp
    ../shotwin.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/models")
target_link_libraries(${PROJECT_NAME} GTest::main Qt5::Widgets Qt5::Sql Exiv2::exiv2)
