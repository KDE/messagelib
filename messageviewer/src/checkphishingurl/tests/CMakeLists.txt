set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include(ECMAddTests)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

set(checkphishingurlgui_SRCS checkphishingurlgui.cpp)
add_executable(checkphishingurlgui ${checkphishingurlgui_SRCS})
target_link_libraries(checkphishingurlgui
    KF5::KIOCore
    KF5::MessageViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )
