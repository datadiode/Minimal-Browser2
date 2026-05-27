cmake_minimum_required(VERSION 3.16)
project(Minimal_Browser VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui PrintSupport WebEngineWidgets Widgets)

qt_standard_project_setup()

qt_add_executable(Minimal_Browser WIN32 MACOSX_BUNDLE
    form.cpp form.h form.ui
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
)
target_compile_definitions(Minimal_Browser PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(Minimal_Browser PRIVATE
    Qt::Core
    Qt::Gui
    Qt::PrintSupport
    Qt::WebEngineWidgets
    Qt::Widgets
)

install(TARGETS Minimal_Browser
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET Minimal_Browser
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
