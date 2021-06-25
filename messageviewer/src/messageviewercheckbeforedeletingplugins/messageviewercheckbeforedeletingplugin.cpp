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
    bool mIsEnabled = true;
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

void MessageViewerCheckBeforeDeletingPlugin::setIsEnabled(bool enabled)
{
    d->mIsEnabled = enabled;
}

bool MessageViewerCheckBeforeDeletingPlugin::isEnabled() const
{
    return d->mIsEnabled;
}
