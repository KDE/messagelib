# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(webengineembedparttest.cpp  ../webengineembedpart.cpp webengineembedparttest.h
    TEST_NAME webengineembedparttest
    NAME_PREFIX "messageviewer-htmlwriter-"
    LINK_LIBRARIES Qt::Test Qt::Core
)

