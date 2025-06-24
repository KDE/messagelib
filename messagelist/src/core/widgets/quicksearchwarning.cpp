/*
  SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

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
    setPosition(KMessageWidget::Header);
    setText(i18n("The words less than 3 letters are ignored."));
    auto action = new QAction(i18nc("@action", "Do not show again"), this);
    action->setObjectName(QLatin1StringView("donotshowagain"));
    connect(action, &QAction::triggered, this, &QuickSearchWarning::slotDoNotRememberIt);
    addAction(action);
}

QuickSearchWarning::~QuickSearchWarning() = default;

void QuickSearchWarning::setSearchText(const QString &text)
{
    if (!MessageList::MessageListSettings::quickSearchWarningDoNotShowAgain()) {
        const QList<QStringView> lstText = QStringView(text).split(u' ', Qt::SkipEmptyParts);
        bool foundLessThanThreeCharacters = false;
        for (const QStringView splitText : lstText) {
            if (splitText.trimmed().size() < 3) {
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

#include "moc_quicksearchwarning.cpp"
