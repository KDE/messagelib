set(dkimtest_lookupkey_SRCS main.cpp lookupkey.cpp)
add_executable(dkimtest_lookupkey ${dkimtest_lookupkey_SRCS})
target_link_libraries(dkimtest_lookupkey
	Qt5::Network
)

