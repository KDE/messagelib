add_library(messageviewer_grantlee_extension MODULE grantleeextension.cpp)
target_link_libraries(messageviewer_grantlee_extension Grantlee5::Templates)
install(TARGETS messageviewer_grantlee_extension DESTINATION ${KDE_INSTALL_PLUGINDIR}/messageviewer/grantlee/5.0)
