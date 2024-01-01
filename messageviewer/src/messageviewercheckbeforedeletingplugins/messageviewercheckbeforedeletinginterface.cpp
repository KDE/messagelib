/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletinginterface.h"
#include "messageviewercheckbeforedeletingparameters.h"

#include <QAction>

using namespace MessageViewer;

class MessageViewer::MessageViewerCheckBeforeDeletingInterfacePrivate
{
public:
    MessageViewerCheckBeforeDeletingInterfacePrivate() = default;

    MessageViewer::MessageViewerCheckBeforeDeletingParameters parameters;
    QWidget *mParentWidget = nullptr;
};

MessageViewerCheckBeforeDeletingInterface::MessageViewerCheckBeforeDeletingInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerCheckBeforeDeletingInterfacePrivate)
{
}

MessageViewerCheckBeforeDeletingInterface::~MessageViewerCheckBeforeDeletingInterface() = default;

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

QList<QAction *> MessageViewerCheckBeforeDeletingInterface::actions() const
{
    // Reimplement in subclass
    return {};
}

void MessageViewerCheckBeforeDeletingInterface::createActions(KActionCollection *ac)
{
    Q_UNUSED(ac);
}

#include "moc_messageviewercheckbeforedeletinginterface.cpp"
