# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include(ECMAddTests)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_definitions( -DCHECKPHISHINGURL_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
add_executable(checkphishingurlgui checkphishingurlgui.cpp checkphishingurlgui.h)
target_link_libraries(checkphishingurlgui
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF${KF_MAJOR_VERSION}::I18n
    )

add_executable(searchfullhashgui searchfullhashgui.cpp searchfullhashgui.h)
target_link_libraries(searchfullhashgui
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF${KF_MAJOR_VERSION}::I18n
    )

add_executable(createphishingurldatabasegui createphishingurldatabasegui.cpp createphishingurldatabasegui.h)
target_link_libraries(createphishingurldatabasegui
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF${KF_MAJOR_VERSION}::I18n
    )

add_executable(managelocaldatabasegui managelocaldatabasegui.cpp managelocaldatabasegui.h)
target_link_libraries(managelocaldatabasegui
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF${KF_MAJOR_VERSION}::I18n
    )

add_executable(webengineviewwithsafebrowsingsupport webengineviewwithsafebrowsingsupport.cpp webengineviewwithsafebrowsingsupport.h)
target_link_libraries(webengineviewwithsafebrowsingsupport
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF${KF_MAJOR_VERSION}::I18n
    )
