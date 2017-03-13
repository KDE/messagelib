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

#ifndef SEARCHLINESTATUS_H
#define SEARCHLINESTATUS_H

#include <PimCommon/LineEditWithCompleter>
#include <Akonadi/KMime/MessageStatus>
#include "messagelist_private_export.h"
#include "quicksearchline.h"
#include <QIcon>
class QAction;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchLineStatus : public PimCommon::LineEditWithCompleter
{
    Q_OBJECT
public:
    explicit SearchLineStatus(QWidget *parent = nullptr);
    ~SearchLineStatus();

    void setLocked(bool b);
    bool locked() const;

    void clearFilterAction();

    void setContainsOutboundMessages(bool containsOutboundMessages);
    bool containsOutboundMessages() const;
    QuickSearchLine::SearchOptions searchOptions() const;
Q_SIGNALS:
    void filterActionChanged(const QList<Akonadi::MessageStatus> &lst);
    void searchOptionChanged();

private Q_SLOTS:
    void slotToggledLockAction();
    void showMenu();
    void clearFilterButtonClicked();
    void slotSearchOptionChanged();
    void slotFilterActionClicked(QAction *act);
private:
    void createFilterAction(const QIcon &icon, const QString &text, int value);
    void createMenuSearch();
    void updateLockAction();
    void initializeActions();
    void updateFilterActionIcon();
    void updateFilters();
    void createFilterByAction();
    void clearFilterByAction();
    void changeSearchAgainstFromOrToText();

    bool mLocked;
    bool mHasFilter;
    QIcon mWithoutFilter;
    QIcon mWithFilter;
    QAction *mLockAction;
    QAction *mFiltersAction;
    QMenu *mFilterMenu;
    QList<QAction *> mFilterListActions;
    QAction *mSearchEveryWhereAction;
    QAction *mSearchAgainstBodyAction;
    QAction *mSearchAgainstSubjectAction;
    QAction *mSearchAgainstFromOrToAction;
    QAction *mSearchAgainstBccAction;
    bool mContainsOutboundMessages;
};

}
}

#endif // SEARCHLINESTATUS_H
