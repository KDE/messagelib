# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include(ECMAddTests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_definitions(-DCHECKPHISHINGURL_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_executable(checkphishingurlgui checkphishingurlgui.cpp checkphishingurlgui.h)
target_link_libraries(checkphishingurlgui
    KPim6::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF6::I18n
)

add_executable(searchfullhashgui searchfullhashgui.cpp searchfullhashgui.h)
target_link_libraries(searchfullhashgui
    KPim6::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF6::I18n
)

add_executable(createphishingurldatabasegui createphishingurldatabasegui.cpp createphishingurldatabasegui.h)
target_link_libraries(createphishingurldatabasegui
    KPim6::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF6::I18n
)

add_executable(managelocaldatabasegui managelocaldatabasegui.cpp managelocaldatabasegui.h)
target_link_libraries(managelocaldatabasegui
    KPim6::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF6::I18n
)

add_executable(webengineviewwithsafebrowsingsupport webengineviewwithsafebrowsingsupport.cpp webengineviewwithsafebrowsingsupport.h)
target_link_libraries(webengineviewwithsafebrowsingsupport
    KPim6::WebEngineViewer
    
    Qt::WebEngineWidgets
    KF6::I18n
)
