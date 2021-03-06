include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CORBASIM_PREFIX corbasim_)
set(corbasim_app_SRCS
    AppMainWindow.cpp
    AppConfiguration.cpp

    ObjrefView.cpp
    ServantView.cpp
)

set(corbasim_app_MOC_HDRS
    AppMainWindow.hpp

    ObjrefView.hpp
    ServantView.hpp
)

set(corbasim_app_FORMS
    AppMainWindow.ui
)

qt4_wrap_ui(corbasim_app_FORMS_HDRS ${corbasim_app_FORMS})

# NOTE: QT bug: https://bugreports.qt.nokia.com/browse/QTBUG-22829
qt4_wrap_cpp(corbasim_app_MOC_SRCS
    ${corbasim_app_MOC_HDRS} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

add_executable(corbasim_app.real
    main.cpp
    ${corbasim_app_FORMS_HDRS}
    ${corbasim_app_MOC_SRCS}
    ${corbasim_app_SRCS})
target_link_libraries(corbasim_app.real
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    corbasim
    corbasim_qt
    corbasim_gui
    corbasim_reflective_CosNaming_NamingContextExt)

install(TARGETS corbasim_app.real DESTINATION bin)

# Launcher
add_executable(corbasim_app
    ${CMAKE_CURRENT_SOURCE_DIR}/../corbasim/gui/launcher.cpp
)
target_link_libraries(corbasim_app
    ${QT_LIBRARIES}
)
install(TARGETS corbasim_app DESTINATION bin)

set(corbasim_ui_SRCS
)

add_executable(corbasim_ui.real
    main_ui.cpp
    ${corbasim_ui_SRCS})
target_link_libraries(corbasim_ui.real
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    corbasim
    corbasim_qt
    corbasim_gui
    corbasim_reflective_CosNaming_NamingContextExt)

install(TARGETS corbasim_ui.real DESTINATION bin)

# Launcher
add_executable(corbasim_ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../corbasim/gui/launcher.cpp
)
target_link_libraries(corbasim_ui
    ${QT_LIBRARIES}
)
install(TARGETS corbasim_ui DESTINATION bin)

