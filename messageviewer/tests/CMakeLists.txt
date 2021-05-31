add_executable(viewertest_gui)
target_sources(viewertest_gui PRIVATE
    viewertest_gui.cpp
    ../src/messageviewer_debug.cpp)
target_link_libraries(viewertest_gui
  KF5::Mime
  KF5::MessageViewer
  KF5::GrantleeTheme
  KF5::I18n
  KF5::XmlGui
)

