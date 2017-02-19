ecm_add_test(attachmenttemporaryfilesdirstest.cpp
    NAME_PREFIX "mimeptreeparser-temporaryfile-"
    LINK_LIBRARIES Qt5::Test QGpgme KF5::MimeTreeParser
)
