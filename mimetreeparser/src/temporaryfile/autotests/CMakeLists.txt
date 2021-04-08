ecm_add_test(attachmenttemporaryfilesdirstest.cpp
    NAME_PREFIX "mimeptreeparser-temporaryfile-"
    LINK_LIBRARIES Qt::Test KF5::MimeTreeParser QGpgme
)
