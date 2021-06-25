/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletingplugin.h"

using namespace MessageViewer;
class MessageViewer::MessageViewerCheckBeforeDeletingPluginPrivate
{
public:
    MessageViewerCheckBeforeDeletingPluginPrivate()
    {
    }
};

MessageViewerCheckBeforeDeletingPlugin::MessageViewerCheckBeforeDeletingPlugin(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerCheckBeforeDeletingPluginPrivate)
{
}

MessageViewerCheckBeforeDeletingPlugin::~MessageViewerCheckBeforeDeletingPlugin()
{
    delete d;
}

void MessageViewerCheckBeforeDeletingPlugin::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
    // Reimplement
}
