/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletingplugin.h"

using namespace MessageViewer;
class MessageViewer::MessageViewerCheckBeforeDeletingPluginPrivate
{
public:
    MessageViewerCheckBeforeDeletingPluginPrivate() = default;
    bool mIsEnabled = true;
};

MessageViewerCheckBeforeDeletingPlugin::MessageViewerCheckBeforeDeletingPlugin(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerCheckBeforeDeletingPluginPrivate)
{
}

MessageViewerCheckBeforeDeletingPlugin::~MessageViewerCheckBeforeDeletingPlugin() = default;

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

#include "moc_messageviewercheckbeforedeletingplugin.cpp"
