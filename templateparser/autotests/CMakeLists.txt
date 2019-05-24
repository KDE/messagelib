include(ECMAddTests)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

include ( ${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake )

# convenience macro to add qtest unit tests
macro(add_templateparser_unittest _source)
  set(_test ${_source})
  get_filename_component(_name ${_source} NAME_WE)
  add_executable( ${_name} ${_test} )
  ecm_mark_as_test(templateparser-${_name})
  target_link_libraries(${_name}
    KF5::Libkleo
    KF5::TemplateParser
    KF5::MimeTreeParser
    QGpgme
    Qt5::Test
    KF5::KIOCore
    KF5::Mime
    KF5::IdentityManagement
    KF5::XmlGui
    KF5::Completion
    KF5::IconThemes
    Qt5::WebEngineWidgets
  )
  add_gpg_crypto_test(${_name} templateparser-${_name})
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
