# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(
    dkimtest_lookupkey
    main.cpp
    lookupkey.cpp
    lookupkey.h
)
target_link_libraries(
    dkimtest_lookupkey
    Qt::Network
    OpenSSL::SSL
)

add_executable(
    dkimtest_checkrsapublickey
    checkrsapublickey.cpp
    checkrsapublickey.h
)
target_link_libraries(
    dkimtest_checkrsapublickey
    Qt::Network
    OpenSSL::SSL
)

add_executable(
    dkimtest_checksignature
    checksignature.cpp
    checksignature.h
)
target_link_libraries(
    dkimtest_checksignature
    Qt::Network
    KPim6::MessageCore
    OpenSSL::SSL
)

add_executable(
    dkimtest_dmarclookupkey
    dmarclookupkey.cpp
    dmarclookupkey.h
)
target_link_libraries(
    dkimtest_dmarclookupkey
    Qt::Network
    KPim6::MessageCore
    OpenSSL::SSL
)
