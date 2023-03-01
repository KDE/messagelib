# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
include(ECMAddTests)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(templateeditor_gui_SRCS templateeditor_gui.cpp templateeditor_gui.h)
add_executable(templateeditor_gui ${templateeditor_gui_SRCS})
target_link_libraries(templateeditor_gui
  KF${KF_MAJOR_VERSION}::KIOCore
  KPim${KF_MAJOR_VERSION}::TemplateParser
  KPim${KF_MAJOR_VERSION}::PimTextEdit
  KF${KF_MAJOR_VERSION}::I18n
  KPim${KF_MAJOR_VERSION}::MessageViewer
)

set(templateconfigurewidget_gui_SRCS templateconfigurewidget_gui.cpp templateconfigurewidget_gui.h)
add_executable(templateconfigurewidget_gui ${templateconfigurewidget_gui_SRCS})
target_link_libraries(templateconfigurewidget_gui
  KF${KF_MAJOR_VERSION}::KIOCore
  KPim${KF_MAJOR_VERSION}::TemplateParser
  KF${KF_MAJOR_VERSION}::I18n
  KPim${KF_MAJOR_VERSION}::PimTextEdit
  KPim${KF_MAJOR_VERSION}::MessageViewer
)

set(templateparseremailrequester_gui_SRCS templateparseremailrequester.cpp templateparseremailrequester.h)
add_executable(templateparseremailrequester_gui ${templateparseremailrequester_gui_SRCS})
target_link_libraries(templateparseremailrequester_gui
  KPim${KF_MAJOR_VERSION}::TemplateParser
  KF${KF_MAJOR_VERSION}::I18n
  KPim${KF_MAJOR_VERSION}::MessageViewer
  KPim${KF_MAJOR_VERSION}::PimTextEdit
)

