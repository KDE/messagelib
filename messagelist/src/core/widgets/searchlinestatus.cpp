/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "searchlinestatus.h"
#include <KLocalizedString>
#include <QAction>
#include <QStandardPaths>
#include <QMenu>
#include <KIconEngine>
#include <KIconLoader>

using namespace MessageList::Core;
SearchLineStatus::SearchLineStatus(QWidget *parent)
    : PimCommon::LineEditWithCompleter(parent),
      mLocked(false),
      mHasFilter(false),
      mLockAction(Q_NULLPTR),
      mFiltersAction(Q_NULLPTR),
      mFilterMenu(Q_NULLPTR)
{
    setClearButtonEnabled(true);
    setClearButtonShown(false);
    initializeActions();
    createMenuSearch();
}

SearchLineStatus::~SearchLineStatus()
{

}

void SearchLineStatus::updateLockAction()
{
    if (mLocked) {
        mLockAction->setIcon(QIcon::fromTheme(QStringLiteral("object-locked")));
        mLockAction->setToolTip(i18nc("@info:tooltip",
                                      "Prevent the quick search field from being cleared when changing folders"));
    } else {
        mLockAction->setIcon(QIcon::fromTheme(QStringLiteral("object-unlocked")));
        mLockAction->setToolTip(i18nc("@info:tooltip", "Clear the quick search field when changing folders"));
    }
}

void SearchLineStatus::setLocked(bool b)
{
    if (mLocked != b) {
        slotToggledLockAction();
    }
}

bool SearchLineStatus::locked() const
{
    return mLocked;
}

void SearchLineStatus::initializeActions()
{
    mLockAction = addAction(QIcon::fromTheme(QStringLiteral("object-locked")), QLineEdit::TrailingPosition);
    mLockAction->setWhatsThis(
        i18nc("@info:whatsthis",
              "Toggle this button if you want to keep your quick search "
              "locked when moving to other folders or when narrowing the search "
              "by message status."));

    connect(mLockAction, &QAction::triggered, this, &SearchLineStatus::slotToggledLockAction);
    updateLockAction();

    const QStringList overlays = QStringList() << QStringLiteral("list-add");
    mWithFilter = QIcon(new KIconEngine(QStringLiteral("view-filter"), KIconLoader::global(), overlays));
    mWithoutFilter = QIcon::fromTheme(QStringLiteral("view-filter"));
    mFiltersAction = addAction(mWithoutFilter, QLineEdit::LeadingPosition);
    connect(mFiltersAction, &QAction::triggered, this, &SearchLineStatus::showMenu);
}

void SearchLineStatus::slotToggledLockAction()
{
    mLocked = !mLocked;
    updateLockAction();
}

void SearchLineStatus::showMenu()
{
    if (mFilterMenu->exec(mapToGlobal(QPoint(0, height())))) {
        QList<Akonadi::MessageStatus> lstStatus;

        Q_FOREACH (QAction *act, mFilterListActions ) {
            if (act->isChecked()) {
                Akonadi::MessageStatus status;
                status.fromQInt32(static_cast< qint32 >(act->data().toInt()));
                lstStatus.append(status);
            }
        }
        mHasFilter = !lstStatus.isEmpty();
        Q_EMIT filterActionChanged(lstStatus);
        updateFilterActionIcon();
    }
}

void SearchLineStatus::clearFilterAction()
{
    Q_FOREACH (QAction *act, mFilterListActions ) {
        act->setChecked(false);
    }
    updateFilterActionIcon();
}

void SearchLineStatus::createFilterAction(const QIcon &icon, const QString &text, int value)
{
    QAction *act = new QAction(icon, text, this);
    act->setCheckable(true);
    act->setData(value);
    mFilterMenu->addAction(act);
    mFilterListActions.append(act);
}

void SearchLineStatus::updateFilterActionIcon()
{
    mFiltersAction->setIcon(mHasFilter ? mWithFilter : mWithoutFilter);
}

void SearchLineStatus::createMenuSearch()
{
    mFilterMenu = new QMenu(this);
    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-unread")), i18nc("@action:inmenu Status of a message", "Unread"),
                            Akonadi::MessageStatus::statusUnread().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-replied")),
                            i18nc("@action:inmenu Status of a message", "Replied"),
                            Akonadi::MessageStatus::statusReplied().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-forwarded")),
                            i18nc("@action:inmenu Status of a message", "Forwarded"),
                            Akonadi::MessageStatus::statusForwarded().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("emblem-important")),
                            i18nc("@action:inmenu Status of a message", "Important"),
                            Akonadi::MessageStatus::statusImportant().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-task")),
                            i18nc("@action:inmenu Status of a message", "Action Item"),
                            Akonadi::MessageStatus::statusToAct().toQInt32());

    createFilterAction(QIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("messagelist/pics/mail-thread-watch.png"))),
                            i18nc("@action:inmenu Status of a message", "Watched"),
                            Akonadi::MessageStatus::statusWatched().toQInt32());

    createFilterAction(QIcon(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("messagelist/pics/mail-thread-ignored.png"))),
                            i18nc("@action:inmenu Status of a message", "Ignored"),
                            Akonadi::MessageStatus::statusIgnored().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-attachment")),
                            i18nc("@action:inmenu Status of a message", "Has Attachment"),
                            Akonadi::MessageStatus::statusHasAttachment().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-invitation")),
                            i18nc("@action:inmenu Status of a message", "Has Invitation"),
                            Akonadi::MessageStatus::statusHasInvitation().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-mark-junk")),
                            i18nc("@action:inmenu Status of a message", "Spam"),
                            Akonadi::MessageStatus::statusSpam().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-mark-notjunk")),
                            i18nc("@action:inmenu Status of a message", "Ham"),
                            Akonadi::MessageStatus::statusHam().toQInt32());
}
