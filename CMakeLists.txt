cmake_minimum_required(VERSION 3.7)

project(windeployqt)

add_executable(windeployqt 
    elfreader.cpp elfreader.h
    qmlutils.cpp qmlutils.h
    utils.cpp utils.h
    main.cpp
)

find_package(Qt5Core REQUIRED)

target_link_libraries(windeployqt PUBLIC Qt5::CorePrivate Qt5::Core shlwapi)
target_compile_definitions(windeployqt PUBLIC QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_FOREACH UNICODE)

install(TARGETS windeployqt)
