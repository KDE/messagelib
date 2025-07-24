# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
kcoreaddons_add_plugin(messageviewer_defaultgrantleeheaderstyleplugin
    INSTALL_NAMESPACE pim6/messageviewer/headerstyle
)

target_sources(messageviewer_defaultgrantleeheaderstyleplugin PRIVATE
   defaultgrantleeheaderstyleplugin.cpp
   defaultgrantleeheaderstyleinterface.cpp
   defaultgrantleeheaderstrategy.cpp
   defaultgrantleeheaderstyleplugin.h
   defaultgrantleeheaderstyleinterface.h
   defaultgrantleeheaderstrategy.h
)

kconfig_add_kcfg_files(messageviewer_defaultgrantleeheaderstyleplugin
  settings/globalsettings_base.kcfgc
)

target_link_libraries(messageviewer_defaultgrantleeheaderstyleplugin
  KPim6::MessageViewer
  KPim6::GrantleeTheme
  KF6::XmlGui
)

install(DIRECTORY theme/5.2 DESTINATION ${KDE_INSTALL_DATADIR}/messageviewer/defaultthemes/)
