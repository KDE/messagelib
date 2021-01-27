/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewerconfiguresettingsplugin.h"

using namespace MessageViewer;
class MessageViewer::MessageViewerConfigureSettingsPluginPrivate
{
public:
    MessageViewerConfigureSettingsPluginPrivate()
    {
    }
};

MessageViewerConfigureSettingsPlugin::MessageViewerConfigureSettingsPlugin(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerConfigureSettingsPluginPrivate)
{
}

MessageViewerConfigureSettingsPlugin::~MessageViewerConfigureSettingsPlugin()
{
    delete d;
}

void MessageViewerConfigureSettingsPlugin::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
    // Reimplement
}
