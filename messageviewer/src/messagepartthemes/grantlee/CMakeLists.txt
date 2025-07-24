# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_library(messageviewer_ktexttemplate_extension MODULE ktexttemplateextension.cpp ktexttemplateextension.h)
target_link_libraries(messageviewer_ktexttemplate_extension KF6::TextTemplate)
install(TARGETS messageviewer_ktexttemplate_extension DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim6/messageviewer/kf6/ktexttemplate)
