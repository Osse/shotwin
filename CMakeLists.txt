cmake_minimum_required(VERSION 3.0)

project(Shotwin)

option(QML_FROM_SOURCE "Load QML files from source" OFF)

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

find_package(Qt5 COMPONENTS Core Widgets Sql QuickWidgets Multimedia)

set(SOURCES
    main.cpp
    mainwindow.cpp
    models/datefilteredeventmodel.cpp
    models/eventitem.cpp
    models/eventmodel.cpp
    models/eventortagfilteredphotomodel.cpp
    models/photoitem.cpp
    models/photomodel.cpp
    models/tagmodel.cpp
    models/treeproxymodel.cpp
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

if(QML_FROM_SOURCE)
    set_source_files_properties(mainwindow.cpp
        PROPERTIES COMPILE_DEFINITIONS
        "CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\""
    )
endif()

get_target_property(libtype Qt5::Core TYPE)
if(libtype STREQUAL "STATIC_LIBRARY")
    get_target_property(qtcorelocation Qt5::Core LOCATION)
    get_filename_component(qtcorepath "${qtcorelocation}" DIRECTORY)
    set(qtprefix "${qtcorepath}/..")
    set(qtqmlprefix "${qtprefix}/share/qt5/qml")
    target_link_libraries(${PROJECT_NAME}
        ${qtqmlprefix}/QtGraphicalEffects/libqtgraphicaleffectsplugin.a
        ${qtqmlprefix}/QtGraphicalEffects/private/libqtgraphicaleffectsprivate.a
        ${qtqmlprefix}/QtMultimedia/libdeclarative_multimedia.a
        ${qtqmlprefix}/QtQuick.2/libqtquick2plugin.a
        ${qtqmlprefix}/QtQuick/Controls/libqtquickcontrolsplugin.a
        ${qtqmlprefix}/QtQuick/Layouts/libqquicklayoutsplugin.a
        ${qtqmlprefix}/QtQuick/Window.2/libwindowplugin.a
        ${qtprefix}/lib/libQt5MultimediaQuick_p.a
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC models)
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Sql
    Qt5::QuickWidgets
    Qt5::Multimedia
)
