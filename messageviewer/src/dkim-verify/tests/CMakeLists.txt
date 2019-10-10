set(dkimtest_lookupkey_SRCS main.cpp lookupkey.cpp)
add_executable(dkimtest_lookupkey ${dkimtest_lookupkey_SRCS})
target_link_libraries(dkimtest_lookupkey
	Qt5::Network
)


set(dkimtest_checkrsapublickey_SRCS checkrsapublickey.cpp)
add_executable(dkimtest_checkrsapublickey ${dkimtest_checkrsapublickey_SRCS})
target_link_libraries(dkimtest_checkrsapublickey
        Qt5::Network
        qca-qt5
)
