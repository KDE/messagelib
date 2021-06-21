set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include(ECMAddTests)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_definitions( -DCHECKPHISHINGURL_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
add_executable(checkphishingurlgui checkphishingurlgui.cpp)
target_link_libraries(checkphishingurlgui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt::WebEngine
    Qt::WebEngineWidgets
    KF5::I18n
    )

add_executable(searchfullhashgui searchfullhashgui.cpp)
target_link_libraries(searchfullhashgui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt::WebEngine
    Qt::WebEngineWidgets
    KF5::I18n
    )

add_executable(createphishingurldatabasegui createphishingurldatabasegui.cpp)
target_link_libraries(createphishingurldatabasegui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt::WebEngine
    Qt::WebEngineWidgets
    KF5::I18n
    )

add_executable(managelocaldatabasegui managelocaldatabasegui.cpp)
target_link_libraries(managelocaldatabasegui
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt::WebEngine
    Qt::WebEngineWidgets
    KF5::I18n
    )

add_executable(webengineviewwithsafebrowsingsupport webengineviewwithsafebrowsingsupport.cpp)
target_link_libraries(webengineviewwithsafebrowsingsupport
    KF5::KIOCore
    KF5::WebEngineViewer
    Qt::WebEngine
    Qt::WebEngineWidgets
    KF5::I18n
    )
