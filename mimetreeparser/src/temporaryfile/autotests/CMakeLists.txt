# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_add_test(attachmenttemporaryfilesdirstest.cpp attachmenttemporaryfilesdirstest.h
    TEST_NAME attachmenttemporaryfilesdirstest
    LINK_LIBRARIES Qt::Test KPim6::MimeTreeParser
)
