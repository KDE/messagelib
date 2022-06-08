# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_library(messageviewer_grantlee_extension MODULE grantleeextension.cpp grantleeextension.h)
target_link_libraries(messageviewer_grantlee_extension ${TEMPLATES_LIB})
install(TARGETS messageviewer_grantlee_extension DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim${QT_MAJOR_VERSION}/messageviewer/grantlee/5.0)
