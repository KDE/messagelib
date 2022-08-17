# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_add_test(attachmenttemporaryfilesdirstest.cpp
    NAME_PREFIX "mimeptreeparser-temporaryfile-"
    LINK_LIBRARIES Qt::Test KF5::MimeTreeParser
)
