/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamdetectionwarningwidget.h"
#include "settings/messageviewersettings.h"

#include <KLocalizedString>
#include <QAction>
#include <QMenu>

using namespace MessageViewer;

class MessageViewer::ScamDetectionWarningWidgetPrivate
{
public:
    ScamDetectionWarningWidgetPrivate()
    {
    }

    bool mUseInTestApps = false;
};

ScamDetectionWarningWidget::ScamDetectionWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
    , d(new MessageViewer::ScamDetectionWarningWidgetPrivate)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    setText(i18n("This message may be a scam. <a href=\"scamdetails\">(Details...)</a>"));

    connect(this, &ScamDetectionWarningWidget::linkActivated, this, &ScamDetectionWarningWidget::slotShowDetails);

    auto menu = new QMenu(this);
    auto action = new QAction(i18n("Move to Trash"), this);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::moveMessageToTrash);
    action->setMenu(menu);
    addAction(action);

    action = new QAction(i18n("I confirm it's not a scam"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::slotMessageIsNotAScam);

    action = new QAction(i18n("Add email to whitelist"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::slotAddToWhiteList);

    action = new QAction(i18n("Disable scam detection for all messages"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, &ScamDetectionWarningWidget::slotDisableScamDetection);
}

ScamDetectionWarningWidget::~ScamDetectionWarningWidget()
{
    delete d;
}

void ScamDetectionWarningWidget::setUseInTestApps(bool b)
{
    d->mUseInTestApps = b;
}

void ScamDetectionWarningWidget::slotMessageIsNotAScam()
{
    Q_EMIT messageIsNotAScam();
    setVisible(false);
}

void ScamDetectionWarningWidget::slotShowDetails(const QString &content)
{
    if (content == QLatin1String("scamdetails")) {
        Q_EMIT showDetails();
    }
}

void ScamDetectionWarningWidget::slotShowWarning()
{
    animatedShow();
}

void ScamDetectionWarningWidget::slotDisableScamDetection()
{
    if (!d->mUseInTestApps) {
        MessageViewer::MessageViewerSettings::self()->setScamDetectionEnabled(false);
        MessageViewer::MessageViewerSettings::self()->save();
    }
    setVisible(false);
}

void ScamDetectionWarningWidget::slotAddToWhiteList()
{
    setVisible(false);
    Q_EMIT addToWhiteList();
}
