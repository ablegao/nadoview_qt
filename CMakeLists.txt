cmake_minimum_required(VERSION 3.16)
#include("cmake/HunterGate.cmake")
include("FetchContent")
#HunterGate(
#    URL "https://github.com/cpp-pm/hunter/archive/v0.24.17.tar.gz"
#    SHA1 "e6396699e414120e32557fe92db097b7655b760b"
#)
project(
  nadoview
  VERSION 0.1
  LANGUAGES CXX)



set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")

# set(QT_DEBUG_PLUGINS 1)

set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/icon.icns")
set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/app_win32.rc")



set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                         "Resources")


FetchContent_Declare(
    quazip
    GIT_REPOSITORY https://github.com/stachenov/quazip.git
    GIT_TAG master
)

## aws sdk 会执行test
#option(ENABLE_TESTING "" OFF)
#option(AUTORUN_UNIT_TESTS "" OFF)

#FetchContent_Declare(
#    aws-sdk-cpp
#    GIT_REPOSITORY https://github.com/aws/aws-sdk-cpp.git
#    GIT_TAG 1.11.92
#    EXCLUDE_FROM_ALL TRUE
#)

FetchContent_MakeAvailable(quazip)

#add_subdirectory(aws-sdk-cpp EXCLUDE_FROM_ALL)



find_package(
  Qt6 6.5.1
  COMPONENTS Core
             WebView
             WebChannel
             HttpServer
             QuickControls2
             Widgets
             QuickControls2Impl
             Sql
             GUI
             Quick
             xml
  REQUIRED)
qt_add_resources(RESOURCE_FILES res.qrc)

qt_add_executable(
  appnadoview
  ${RESOURCE_FILES}
  myapplication.h
  myapplication.cpp
  openchatmodel.h
  openchatmodel.cpp
  userdata.h
  userdata.cpp
  userdatabookworks.h
  userdatabookworks.cpp
  painterimage.h
  painterimage.cpp
  tableofcontent.h
  tableofcontent.cpp
  ebooklib/ibook.h
  ebooklib/ibook.cpp
  myappsingle.h
  myappsingle.cpp
#  macos.h
#  macos.mm
  transferworker.h
  transferworker.cpp
  transfer.h
  transfer.cpp
  ebooklib/epubbook.h
  ebooklib/epubbook.cpp
  main.cpp
  Info.plist
  # MANUAL_FINALIZATION
  ${app_icon_macos}
  ${app_icon_resource_windows})

qt_add_qml_module(
  appnadoview
  URI
  nadoview
  VERSION
  1.0
  QML_FILES
  main.qml
  books.qml)

set_target_properties(
  appnadoview
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER com.tap2happy.nadoview
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
             MACOSX_BUNDLE_ICON_FILE icon.icns
             MACOSX_BUNDLE TRUE
             # QT_DEBUG_PLUGINS TRUE
             WIN32_EXECUTABLE TRUE
             IMPORTED_LOCATION  ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib/libaws-cpp-sdk-core.a
             IMPORTED_LOCATION  ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib/aws-cpp-sdk-translate.a
#             INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/include
         )

set_property(
  TARGET appnadoview
  APPEND
  PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)

target_link_libraries(
  appnadoview
  PRIVATE Qt6::Core
          Qt6::CorePrivate
          Qt6::WebView
          Qt6::WebChannel
#          Qt6::WebEngineQuick
          Qt6::HttpServer
          Qt6::QuickControls2
          Qt6::QuickControls2Impl
          Qt6::Widgets
          Qt6::Sql
          Qt6::Gui
          Qt6::Quick
          Qt6::Xml
          QuaZip::QuaZip

#          aws-cpp-sdk-core
##                  # aws-cpp-sdk-comprehend
#          aws-cpp-sdk-translate
#          ${quazip_BINARY_DIR}/quazip/libquazip1-qt6d.1.4.dylib
      )

install(
  TARGETS appnadoview
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
