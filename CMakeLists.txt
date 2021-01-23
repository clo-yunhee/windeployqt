add_executable(windeployqt 
    ../shared/winutils/elfreader.cpp ../shared/winutils/elfreader.h
    ../shared/winutils/qmlutils.cpp ../shared/winutils/qmlutils.h
    ../shared/winutils/utils.cpp ../shared/winutils/utils.h
    main.cpp
)

find_package(Qt5 REQUIRED COMPONENTS Core)

target_link_libraries(windeployqt Qt::CorePrivate Qt::Core)
