cmake_minimum_required(VERSION 3.0)

project(Shotwin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GetGitRevisionDescription)
git_describe(GIT_VERSION --always --dirty)
configure_file(git_version.h.in git_version.h)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc and uic automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Widgets Sql QuickWidgets Multimedia)
find_package(Exiv2 REQUIRED)

set(SOURCES
    framegrabber.cpp
    main.cpp
    mainwindow.cpp
    models/eventitem.cpp
    models/eventtreeitem.cpp
    models/eventtreemodel.cpp
    models/eventortagfilteredphotomodel.cpp
    models/hidephotosproxymodel.cpp
    models/monthitem.cpp
    models/photoitem.cpp
    models/photomodel.cpp
    models/rootitem.cpp
    models/yearitem.cpp
    settingsdialog.cpp
    shotwin.cpp
    thumbnailprovider.cpp
)

set(UIFORMS
    mainwindow.ui
    settingsdialog.ui
)

set(RESOURCES
    resources.qrc
)

set(QMLFILES
    BorderedImage.qml
    CenteredGridView.qml
    FullScreenView.qml
    EventView.qml
    Main.qml
    PhotoView.qml
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${UIFORMS}
    ${RESOURCES}
    ${QMLFILES}
)

add_definitions("-DCMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
target_include_directories(${PROJECT_NAME} PUBLIC models)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Sql Qt5::QuickWidgets Qt5::Multimedia Exiv2::exiv2)
