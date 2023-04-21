cmake_minimum_required(VERSION 3.16)

project(nadoview VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")

#set(QT_DEBUG_PLUGINS 1)

set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/icon.icns")
set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/app_win32.rc")

set_source_files_properties(${app_icon_macos} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")



add_subdirectory(quazip)
find_package(Qt6 6.5.0 COMPONENTS Core WebView HttpServer QuickControls2 Widgets QuickControls2Impl Sql GUI Quick xml REQUIRED)
qt_add_resources(RESOURCE_FILES res.qrc)

qt_add_executable(appnadoview
    ${RESOURCE_FILES}
    myapplication.h
    myapplication.cpp
    openchatmodel.h
    openchatmodel.cpp
#    mybookurlschemehandler.h
#    mybookurlschemehandler.cpp
    userdata.h
    userdata.cpp
    painterimage.h
    painterimage.cpp
    tableofcontent.h
    tableofcontent.cpp
    ebooklib/ibook.h
    ebooklib/ibook.cpp
    mybookimageschemehandler.h
    mybookimageschemehandler.cpp
    networkmanager.h
    networkmanager.cpp

#    customurlschemehandler.h
#    customurlschemehandler.cpp
    ebooklib/epubbook.h
    ebooklib/epubbook.cpp
    main.cpp
    Info.plist
#    MANUAL_FINALIZATION
    ${app_icon_macos}
    ${app_icon_resource_windows}
)

qt_add_qml_module(appnadoview
    URI nadoview
    VERSION 1.0
    QML_FILES mainwin.qml main.qml books.qml
)

set_target_properties(appnadoview PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_ICON_FILE icon.icns
    MACOSX_BUNDLE TRUE
#    QT_DEBUG_PLUGINS TRUE
    WIN32_EXECUTABLE TRUE
)

set_property(TARGET appnadoview APPEND PROPERTY
           QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
       )

target_link_libraries(appnadoview
    PRIVATE Qt6::Core Qt6::CorePrivate Qt6::WebView Qt6::HttpServer Qt6::QuickControls2 Qt6::QuickControls2Impl Qt6::Widgets Qt6::Sql Qt6::Gui  Qt6::Quick Qt6::Xml  QuaZip::QuaZip)

install(TARGETS appnadoview
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
