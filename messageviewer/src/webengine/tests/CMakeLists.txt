set(webengine_test_SRCS
main.cpp
)

add_executable(webenginetest ${webengine_test_SRCS}) 

target_link_libraries(webenginetest
  Qt5::Widgets
)
