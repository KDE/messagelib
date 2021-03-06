
add_definitions(-DTRANSLATION_DOMAIN=\"libmimetreeparser\")

# target_include_directories does not handle empty include paths
include_directories(${GPGME_INCLUDES})

add_library(KF5MimeTreeParser)
add_library(KF5::MimeTreeParser ALIAS KF5MimeTreeParser)

target_sources(KF5MimeTreeParser PRIVATE
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
    memento/cryptobodypartmemento.cpp
    memento/decryptverifybodypartmemento.cpp
    memento/verifydetachedbodypartmemento.cpp
    memento/verifyopaquebodypartmemento.cpp

    temporaryfile/attachmenttemporaryfilesdirs.cpp
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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MimeTreeParser
    COMPONENT Devel
    )

install(FILES
    ${MimeTreeParser_utils_HEADERS}
    ${MimeTreeParser_interfaces_HEADERS}
    ${MimeTreeParser_main_HEADERS}
    ${MimeTreeParser_temporaryfile_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_export.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/mimetreeparser
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

ecm_qt_declare_logging_category(KF5MimeTreeParser HEADER mimetreeparser_debug.h IDENTIFIER MIMETREEPARSER_LOG CATEGORY_NAME org.kde.pim.mimetreeparser
        DESCRIPTION "messagelib (mimetreeparser)"
        OLD_CATEGORY_NAMES log_mimetreeparser
        EXPORT MESSAGELIB
    )


if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MimeTreeParser PROPERTIES UNITY_BUILD ON)
endif()


generate_export_header(KF5MimeTreeParser BASE_NAME mimetreeparser)


target_link_libraries(KF5MimeTreeParser
    PRIVATE
    QGpgme
    KF5::Codecs
    KF5::I18n
    KF5::CoreAddons
    KF5::Mime
    Qt::Gui
    )

install(TARGETS
    KF5MimeTreeParser
    EXPORT KF5MimeTreeParserTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
    )

set_target_properties(KF5MimeTreeParser PROPERTIES
    VERSION ${MIMETREEPARSER_VERSION}
    SOVERSION ${MIMETREEPARSER_SOVERSION}
    EXPORT_NAME MimeTreeParser
    )

target_include_directories(KF5MimeTreeParser INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MimeTreeParser/;${KDE_INSTALL_INCLUDEDIR_KF5}/mimetreeparser>")

ecm_generate_pri_file(BASE_NAME MimeTreeParser
    LIB_NAME KF5MimeTreeParser
    FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MimeTreeParser
    )

