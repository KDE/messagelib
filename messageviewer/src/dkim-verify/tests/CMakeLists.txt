# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(dkimtest_lookupkey_SRCS main.cpp lookupkey.cpp lookupkey.h)
add_executable(dkimtest_lookupkey ${dkimtest_lookupkey_SRCS})
target_link_libraries(dkimtest_lookupkey
	Qt::Network
    OpenSSL::SSL
)

#####################
set(dkimtest_checkrsapublickey_SRCS checkrsapublickey.cpp checkrsapublickey.h)
add_executable(dkimtest_checkrsapublickey ${dkimtest_checkrsapublickey_SRCS})
target_link_libraries(dkimtest_checkrsapublickey
        Qt::Network
        OpenSSL::SSL
)
#####################
set(dkimtest_checksignature_SRCS checksignature.cpp checksignature.h)
add_executable(dkimtest_checksignature ${dkimtest_checksignature_SRCS})
target_link_libraries(dkimtest_checksignature
        Qt::Network
        KPim6::MessageViewer
        OpenSSL::SSL
)

#####################
set(dkimtest_dmarclookupkey_SRCS dmarclookupkey.cpp dmarclookupkey.h)
add_executable(dkimtest_dmarclookupkey ${dkimtest_dmarclookupkey_SRCS})
target_link_libraries(dkimtest_dmarclookupkey
        Qt::Network
        KPim6::MessageViewer
        OpenSSL::SSL
)

#####################
add_executable(dkimmanagerkeydialog_gui)
target_sources(dkimmanagerkeydialog_gui PRIVATE dkimmanagerkeydialog_gui.cpp)
target_link_libraries(dkimmanagerkeydialog_gui
        Qt::Network
        KPim6::MessageViewer
        OpenSSL::SSL
)

