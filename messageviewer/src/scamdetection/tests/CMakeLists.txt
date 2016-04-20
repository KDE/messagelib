set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include(ECMAddTests)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

set(scamdetectionwebengine_gui_SRCS scamdetectionwebengine_gui.cpp)
add_executable(scamdetectionwebengine_gui ${scamdetectionwebengine_gui_SRCS})
target_link_libraries(scamdetectionwebengine_gui
    KF5::KIOCore
    KF5::MessageViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )

