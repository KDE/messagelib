/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletinginterface.h"
#include "messageviewercheckbeforedeletingparameters.h"

using namespace MessageViewer;

class MessageViewer::MessageViewerCheckBeforeDeletingInterfacePrivate
{
public:
    MessageViewerCheckBeforeDeletingInterfacePrivate()
    {
    }

    MessageViewer::MessageViewerCheckBeforeDeletingParameters parameters;
    QWidget *mParentWidget = nullptr;
};

MessageViewerCheckBeforeDeletingInterface::MessageViewerCheckBeforeDeletingInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerCheckBeforeDeletingInterfacePrivate)
{
}

MessageViewerCheckBeforeDeletingInterface::~MessageViewerCheckBeforeDeletingInterface()
{
    delete d;
}

void MessageViewerCheckBeforeDeletingInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *MessageViewerCheckBeforeDeletingInterface::parentWidget() const
{
    return d->mParentWidget;
}

void MessageViewerCheckBeforeDeletingInterface::reloadConfig()
{
    // Reimplement it
}

void MessageViewerCheckBeforeDeletingInterface::setParameters(const MessageViewer::MessageViewerCheckBeforeDeletingParameters &params)
{
    d->parameters = params;
}

MessageViewer::MessageViewerCheckBeforeDeletingParameters MessageViewerCheckBeforeDeletingInterface::parameters() const
{
    return d->parameters;
}
