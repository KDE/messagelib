add_executable(imagescaling_gui imagescaling_gui.cpp)
target_link_libraries(imagescaling_gui
  KF5::KIOCore
  KF5::MessageComposer
  KF5::I18n
)


