set(viewertest_gui_SRCS
   viewertest_gui.cpp
   ../src/messageviewer_debug.cpp
)
add_executable(viewertest_gui ${viewertest_gui_SRCS})
target_link_libraries(viewertest_gui
  KF5::Mime
  KF5::MessageViewer
  KF5::GrantleeTheme
  KF5::I18n
  KF5::XmlGui
)

