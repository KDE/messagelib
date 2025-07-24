# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
include(ECMAddTests)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)
# convenience macro to add qtest unit tests
macro(add_templateparser_unittest _source)
  set(_test ${_source})
  get_filename_component(_name ${_source} NAME_WE)
  add_executable(${_name} ${_test} ${_name}.h)
  ecm_mark_as_test(templateparser-${_name})
  target_link_libraries(${_name}
    KPim6::Libkleo
    KPim6::TemplateParser
    KPim6::MimeTreeParser
    Qt::Test
    KPim6::Mime
    KPim6::IdentityManagementCore
    KF6::XmlGui
    KF6::IconThemes
    Qt::WebEngineWidgets
)
  add_gpg_crypto_test(${_name} templateparser-${_name})
  # needed for the CI to run webengine-related tests
  # the CI runs as root, and webengine insists on disabling sandboxing in that case
  set_tests_properties(templateparser-${_name} PROPERTIES ENVIRONMENT "QTWEBENGINE_DISABLE_SANDBOX=1")

endmacro()

add_templateparser_unittest(customtemplatesmenutest.cpp)
add_templateparser_unittest(templateextracttextfrommailtest.cpp)
add_templateparser_unittest(templateextracthtmlelementfrommailtest.cpp)
add_templateparser_unittest(templateparserjobtest.cpp)
add_templateparser_unittest(templateparserextracthtmlinfotest.cpp)
add_templateparser_unittest(templateparseremailaddressrequesterlineedittest.cpp)
add_templateparser_unittest(templateparseremailaddressrequesterinterfacewidgettest.cpp)
add_templateparser_unittest(templatesinsertcommandactiontest.cpp)
add_templateparser_unittest(templatesinsertcommandpushbuttontest.cpp)
add_templateparser_unittest(templateparserutiltest.cpp)
