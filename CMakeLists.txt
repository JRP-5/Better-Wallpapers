cmake_minimum_required(VERSION 3.16)

project(BetterWallpapers VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_definitions(-DCURL_STATICLIB)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(CURL REQUIRED)

set(PROJECT_SOURCES
        src/mainwindow.cpp
        src/mainwindow.h
        src/bing_wallpaper.cpp
        src/bing_wallpaper.h
        src/wallpaper_utils.cpp
        src/wallpaper_utils.h
        src/unsplash.cpp
        src/unsplash.h
        src/nasa.h src/nasa.cpp
        src/favourites.h src/favourites.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BetterWallpapers
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/main.cpp
        resources.qrc
        README.md





    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BetterWallpapers APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BetterWallpapers SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BetterWallpapers
            ${PROJECT_SOURCES}
        )
    endif()
endif()
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(BetterWallpapers PRIVATE Qt${QT_VERSION_MAJOR}::Widgets CURL::libcurl)



# # Link libcurl
# target_link_libraries(${BetterWallpapers} ${CURL_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BetterWallpapers)
endif()
set_target_properties(BetterWallpapers PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BetterWallpapers
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BetterWallpapers)
endif()
