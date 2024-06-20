# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX MIMETREEPARSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfigVersion.cmake"
    SOVERSION 6
)

add_definitions(-DTRANSLATION_DOMAIN=\"libmimetreeparser6\")

# target_include_directories does not handle empty include paths
include_directories(${GPGME_INCLUDES})

add_library(KPim6MimeTreeParser)
add_library(KPim6::MimeTreeParser ALIAS KPim6MimeTreeParser)

target_sources(KPim6MimeTreeParser PRIVATE
    bodyformatter/applicationpgpencrypted.cpp
    bodyformatter/applicationpkcs7mime.cpp
    bodyformatter/encrypted.cpp
    bodyformatter/mailman.cpp
    bodyformatter/multipartalternative.cpp
    bodyformatter/multipartencrypted.cpp
    bodyformatter/multipartmixed.cpp
    bodyformatter/multipartsigned.cpp
    bodyformatter/textplain.cpp
    bodyformatter/texthtml.cpp
    bodyformatter/utils.cpp
    interfaces/bodypartformatter.cpp
    interfaces/objecttreesource.cpp
    interfaces/bodypart.cpp
    job/qgpgmejobexecutor.cpp
    utils/util.cpp
    bodypartformatter.cpp
    bodypartformatterfactory.cpp
    cryptohelper.cpp
    nodehelper.cpp
    objecttreeparser.cpp
    messagepart.cpp
    partnodebodypart.cpp
    simpleobjecttreesource.cpp
    memento/compositememento.cpp
    memento/cryptobodypartmemento.cpp
    memento/decryptverifybodypartmemento.cpp
    memento/keycachememento.cpp
    memento/verifydetachedbodypartmemento.cpp
    memento/verifyopaquebodypartmemento.cpp

    temporaryfile/attachmenttemporaryfilesdirs.cpp

    job/qgpgmejobexecutor.h
    partnodebodypart.h
    bodypartformatterfactory.h
    utils/util.h
    temporaryfile/attachmenttemporaryfilesdirs.h
    enums.h
    partmetadata.h
    cryptohelper.h
    bodyformatter/utils.h
    bodyformatter/multipartsigned.h
    bodyformatter/encrypted.h
    bodyformatter/applicationpgpencrypted.h
    bodyformatter/texthtml.h
    bodyformatter/multipartencrypted.h
    bodyformatter/multipartalternative.h
    bodyformatter/applicationpkcs7mime.h
    bodyformatter/textplain.h
    bodyformatter/multipartmixed.h
    bodyformatter/mailman.h
    memento/compositememento.h
    memento/cryptobodypartmemento.h
    memento/decryptverifybodypartmemento.h
    memento/keycachememento.h
    memento/verifydetachedbodypartmemento.h
    memento/verifyopaquebodypartmemento.h
    interfaces/bodypart.h
    interfaces/objecttreesource.h
    interfaces/bodypartformatter.h
    simpleobjecttreesource.h
    messagepart.h
    bodypartformatterfactory_p.h
    nodehelper.h
    objecttreeparser.h
)

ecm_generate_headers(MimeTreeParser_Camelcasemain_HEADERS
    HEADER_NAMES
    BodyPartFormatterFactory
    Enums
    MessagePart
    NodeHelper
    ObjectTreeParser
    PartMetaData
    PartNodeBodyPart
    SimpleObjectTreeSource
    REQUIRED_HEADERS MimeTreeParser_main_HEADERS
    PREFIX MimeTreeParser
)

ecm_generate_headers(MimeTreeParser_Camelcaseutils_HEADERS
    HEADER_NAMES
    Util
    REQUIRED_HEADERS MimeTreeParser_utils_HEADERS
    PREFIX MimeTreeParser
    RELATIVE utils
)

ecm_generate_headers(MimeTreeParser_Camelcaseinterfaces_HEADERS
    HEADER_NAMES
    BodyPartFormatter
    BodyPart
    ObjectTreeSource
    REQUIRED_HEADERS MimeTreeParser_interfaces_HEADERS
    PREFIX MimeTreeParser
    RELATIVE interfaces
)

ecm_generate_headers(MimeTreeParser_Camelcasetemporaryfile_HEADERS
    HEADER_NAMES
    AttachmentTemporaryFilesDirs
    REQUIRED_HEADERS MimeTreeParser_temporaryfile_HEADERS
    PREFIX MimeTreeParser
    RELATIVE temporaryfile
)

install(FILES
    ${MimeTreeParser_Camelcaseutils_HEADERS}
    ${MimeTreeParser_Camelcaseinterfaces_HEADERS}
    ${MimeTreeParser_Camelcasemain_HEADERS}
    ${MimeTreeParser_Camelcasetemporaryfile_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MimeTreeParser/MimeTreeParser
    COMPONENT Devel
)

install(FILES
    ${MimeTreeParser_utils_HEADERS}
    ${MimeTreeParser_interfaces_HEADERS}
    ${MimeTreeParser_main_HEADERS}
    ${MimeTreeParser_temporaryfile_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_export.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MimeTreeParser/mimetreeparser
    COMPONENT Devel
)

ecm_qt_declare_logging_category(KPim6MimeTreeParser HEADER mimetreeparser_debug.h IDENTIFIER MIMETREEPARSER_LOG CATEGORY_NAME org.kde.pim.mimetreeparser
        DESCRIPTION "messagelib (mimetreeparser)"
        OLD_CATEGORY_NAMES log_mimetreeparser
        EXPORT MESSAGELIB
)


if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6MimeTreeParser PROPERTIES UNITY_BUILD ON)
endif()

ecm_generate_export_header(KPim6MimeTreeParser
    BASE_NAME mimetreeparser
    VERSION ${MIMETREEPARSER_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)


target_link_libraries(KPim6MimeTreeParser
    PRIVATE
    KF6::Codecs
    KF6::I18n
    KF6::CoreAddons
    KPim6::Libkleo
    KPim6::Mime
    Qt::Gui
    QGpgmeQt6
)

install(TARGETS
    KPim6MimeTreeParser
    EXPORT KPim6MimeTreeParserTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)

set_target_properties(KPim6MimeTreeParser PROPERTIES
    VERSION ${MIMETREEPARSER_VERSION}
    SOVERSION ${MIMETREEPARSER_SOVERSION}
    EXPORT_NAME MimeTreeParser
)

target_include_directories(KPim6MimeTreeParser INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/MimeTreeParser/>")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MimeTreeParser")

if(BUILD_QCH)
    ecm_add_qch(
        KPim6MimeTreeParser_QCH
        NAME KPim6MimeTreeParser
        BASE_NAME KPim6MimeTreeParser
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MimeTreeParser_utils_HEADERS}
        ${MimeTreeParser_interfaces_HEADERS}
        ${MimeTreeParser_main_HEADERS}
        ${MimeTreeParser_temporaryfile_HEADERS}
        LINK_QCHS
            Qt6Core_QCH
            Qt6Gui_QCH
            Qt6Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            MIMETREEPARSER_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
)

    ecm_install_qch_export(
        TARGETS KPim6MimeTreeParser_QCH
        FILE KPim6MimeTreeParserQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
)
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6MimeTreeParserQchTargets.cmake\")")
endif()

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMimeTreeParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim6MimeTreeParserTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MimeTreeParserTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MimeTreeParser COMPONENT Devel
)
