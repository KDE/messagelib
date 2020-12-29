/*
  SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "quicksearchwarning.h"
#include "messagelistsettings.h"
#include <KLocalizedString>
#include <QAction>
using namespace MessageList::Core;

QuickSearchWarning::QuickSearchWarning(QWidget *parent)
    : KMessageWidget(parent)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    setText(i18n("The words less than 3 letters are ignored."));
    auto action = new QAction(i18n("Do not show again"), this);
    action->setObjectName(QStringLiteral("donotshowagain"));
    connect(action, &QAction::triggered, this, &QuickSearchWarning::slotDoNotRememberIt);
    addAction(action);
}

QuickSearchWarning::~QuickSearchWarning()
{
}

void QuickSearchWarning::setSearchText(const QString &text)
{
    if (!MessageList::MessageListSettings::quickSearchWarningDoNotShowAgain()) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        const QStringList lstText = text.split(QLatin1Char(' '), QString::SkipEmptyParts);
#else
        const QStringList lstText = text.split(QLatin1Char(' '), Qt::SkipEmptyParts);
#endif
        bool foundLessThanThreeCharacters = false;
        for (const QString &text : lstText) {
            if (text.trimmed().size() < 3) {
                foundLessThanThreeCharacters = true;
                break;
            }
        }
        if (foundLessThanThreeCharacters) {
            animatedShow();
        } else {
            animatedHide();
        }
    }
}

void QuickSearchWarning::slotDoNotRememberIt()
{
    MessageList::MessageListSettings::setQuickSearchWarningDoNotShowAgain(true);
    animatedHide();
}
