set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include(ECMAddTests)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_definitions( -DCHECKPHISHINGURL_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
set(checkphishingurlgui_SRCS checkphishingurlgui.cpp)
add_executable(checkphishingurlgui ${checkphishingurlgui_SRCS})
target_link_libraries(checkphishingurlgui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )

set(searchfullhashgui_SRCS searchfullhashgui.cpp)
add_executable(searchfullhashgui ${searchfullhashgui_SRCS})
target_link_libraries(searchfullhashgui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )

set(createphishingurldatabasegui_SRCS createphishingurldatabasegui.cpp)
add_executable(createphishingurldatabasegui ${createphishingurldatabasegui_SRCS})
target_link_libraries(createphishingurldatabasegui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )

set(managelocaldatabasegui_SRCS managelocaldatabasegui.cpp)
add_executable(managelocaldatabasegui ${managelocaldatabasegui_SRCS})
target_link_libraries(managelocaldatabasegui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )

set(webengineviewwithsafebrowsingsupport_SRCS webengineviewwithsafebrowsingsupport.cpp)
add_executable(webengineviewwithsafebrowsingsupport ${webengineviewwithsafebrowsingsupport_SRCS})
target_link_libraries(webengineviewwithsafebrowsingsupport
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt5::WebEngine
    Qt5::WebEngineWidgets
    KF5::I18n
    )
