/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mdnwarningwidget.h"
#include <KLocalizedString>
#include <QAction>

using namespace MessageViewer;
MDNWarningWidget::MDNWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
    , mIgnoreAction(new QAction(i18nc("@action", "Ignore"), this))
    , mSendAction(new QAction(i18nc("@action", "Send"), this))
    , mSendDenyAction(new QAction(i18nc("@action", "Send Deny"), this))
{
    setVisible(false);
    setMessageType(Information);
    setWordWrap(true);
    connect(mIgnoreAction, &QAction::triggered, this, &MDNWarningWidget::slotIgnore);
    addAction(mIgnoreAction);
    connect(mSendAction, &QAction::triggered, this, &MDNWarningWidget::slotSend);
    addAction(mSendAction);
    connect(mSendDenyAction, &QAction::triggered, this, &MDNWarningWidget::slotSendDeny);
    addAction(mSendDenyAction);
    mSendDenyAction->setVisible(false); // Hidden by default
    mIgnoreAction->setObjectName(QStringLiteral("mIgnoreAction"));
    mSendAction->setObjectName(QStringLiteral("mSendAction"));
    mSendDenyAction->setObjectName(QStringLiteral("mSendDenyAction"));
}

MDNWarningWidget::~MDNWarningWidget() = default;

void MDNWarningWidget::setCanDeny(bool deny)
{
    mSendDenyAction->setVisible(deny);
}

void MDNWarningWidget::setInformation(const QString &str)
{
    setText(str);
}

void MDNWarningWidget::slotSend()
{
    animatedHide();
    Q_EMIT sendResponse(MessageViewer::MDNWarningWidget::ResponseType::Send);
}

void MDNWarningWidget::slotIgnore()
{
    animatedHide();
    Q_EMIT sendResponse(MessageViewer::MDNWarningWidget::ResponseType::Ignore);
}

void MDNWarningWidget::slotSendDeny()
{
    animatedHide();
    Q_EMIT sendResponse(MessageViewer::MDNWarningWidget::ResponseType::SendDeny);
}
