# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_plugineditor_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-plugineditor-"
        LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::MessageComposer KF5::XmlGui
    )
endmacro()

add_plugineditor_unittest(plugineditortest.cpp)

