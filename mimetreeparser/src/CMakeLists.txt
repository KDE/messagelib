
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )


add_definitions(-DTRANSLATION_DOMAIN=\"libmimetreeparser\")

# target_include_directories does not handle empty include paths
include_directories(${GPGME_INCLUDES})

set(libmimetreeparser_main_SRCS
    bodyformatter/applicationpgpencrypted.cpp
    bodyformatter/applicationpkcs7mime.cpp
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
    interfaces/htmlwriter.cpp
    interfaces/messagepartrenderer.cpp
    job/kleojobexecutor.cpp
    viewer/attachmentstrategy.cpp
    viewer/bodypartformatter.cpp
    viewer/bodypartformatterbasefactory.cpp
    viewer/cryptohelper.cpp
    viewer/csshelperbase.cpp
    viewer/nodehelper.cpp
    viewer/objecttreeparser.cpp
    viewer/messagepart.cpp
    viewer/partnodebodypart.cpp
    viewer/memento/cryptobodypartmemento.cpp
    viewer/memento/decryptverifybodypartmemento.cpp
    viewer/memento/verifydetachedbodypartmemento.cpp
    viewer/memento/verifyopaquebodypartmemento.cpp
    )

set(libmimetreeparser_extra_SRCS
    #for quotes
    #HTML Writer
    htmlwriter/filehtmlwriter.cpp
    htmlwriter/queuehtmlwriter.cpp
    themes/default/converthtmltoplaintext.cpp
    themes/default/defaultrenderer.cpp
    themes/default/htmlblock.cpp
    )

set(mimetreeparser_temporaryfile_SRCS
    temporaryfile/attachmenttemporaryfilesdirs.cpp
    )

ecm_generate_headers(MimeTreeParser_Camelcaseviewer_HEADERS
    HEADER_NAMES
    AttachmentStrategy
    BodyPartFormatterBaseFactory
    CSSHelperBase
    Enums
    MessagePart
    NodeHelper
    ObjectTreeParser
    PartMetaData
    PartNodeBodyPart
    REQUIRED_HEADERS MimeTreeParser_viewer_HEADERS
    PREFIX MimeTreeParser
    RELATIVE viewer
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
    HtmlWriter
    ObjectTreeSource
    REQUIRED_HEADERS MimeTreeParser_interfaces_HEADERS
    PREFIX MimeTreeParser
    RELATIVE interfaces
    )

ecm_generate_headers(MimeTreeParser_Camelcasehtmlwriter_HEADERS
    HEADER_NAMES
    FileHtmlWriter
    REQUIRED_HEADERS MimeTreeParser_htmlwriter_HEADERS
    PREFIX MimeTreeParser
    RELATIVE htmlwriter
    )

ecm_generate_headers(MimeTreeParser_Camelcasetemporaryfile_HEADERS
    HEADER_NAMES
    AttachmentTemporaryFilesDirs
    REQUIRED_HEADERS MimeTreeParser_temporaryfile_HEADERS
    PREFIX MimeTreeParser
    RELATIVE temporaryfile
    )

install(FILES
    ${MimeTreeParser_Camelcasehtmlwriter_HEADERS}
    ${MimeTreeParser_Camelcaseutils_HEADERS}
    ${MimeTreeParser_Camelcaseinterfaces_HEADERS}
    ${MimeTreeParser_Camelcaseviewer_HEADERS}
    ${MimeTreeParser_Camelcasetemporaryfile_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MimeTreeParser
    COMPONENT Devel
    )

install(FILES
    ${MimeTreeParser_htmlwriter_HEADERS}
    ${MimeTreeParser_utils_HEADERS}
    ${MimeTreeParser_interfaces_HEADERS}
    ${MimeTreeParser_viewer_HEADERS}
    ${MimeTreeParser_temporaryfile_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_debug.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/mimetreeparser
    COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME MimeTreeParser
    LIB_NAME KF5MimeTreeParser
    DEPS "MessageCore Libkleo" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MimeTreeParser
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
    )

set(libmimetreeparser_SRCS
    ${libmimetreeparser_main_SRCS}
    ${libmimetreeparser_extra_SRCS}
    ${mimetreeparser_temporaryfile_SRCS}
    )

FILE(GLOB themesfiles_default_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/themes/default/templates/*.html")
install(FILES ${themesfiles_default_SRCS} DESTINATION ${DATA_INSTALL_DIR}/mimetreeparser/themes/default)

ecm_qt_declare_logging_category(libmimetreeparser_SRCS HEADER mimetreeparser_debug.h IDENTIFIER MIMETREEPARSER_LOG CATEGORY_NAME log_mimetreeparser)

add_library(KF5MimeTreeParser
    ${libmimetreeparser_SRCS}
    )

generate_export_header(KF5MimeTreeParser BASE_NAME mimetreeparser)
add_library(KF5::MimeTreeParser ALIAS KF5MimeTreeParser)

target_link_libraries(KF5MimeTreeParser
    PUBLIC
    KF5::Libkleo
    PRIVATE
    KF5::Codecs
    KF5::CoreAddons #KTextToHTML
    KF5::IconThemes
    KF5::I18n
    KF5::MessageCore #StringUtils
    KF5::Mime
    Qt5::WebKitWidgets
    Grantlee5::TextDocument
    Grantlee5::Templates
    )

install(TARGETS
    KF5MimeTreeParser
    EXPORT KF5MimeTreeParserTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )

set_target_properties(KF5MimeTreeParser PROPERTIES
    VERSION ${MIMETREEPARSER_VERSION_STRING}
    SOVERSION ${MIMETREEPARSER_SOVERSION}
    EXPORT_NAME MimeTreeParser
    )

target_include_directories(KF5MimeTreeParser INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MimeTreeParser/;${KDE_INSTALL_INCLUDEDIR_KF5}/mimetreeparser>")
target_include_directories(KF5MimeTreeParser PUBLIC "$<BUILD_INTERFACE:${MimeTreeParser_SOURCE_DIR}/src;${MimeTreeParser_BINARY_DIR}/src;>")
