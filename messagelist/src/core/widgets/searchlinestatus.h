/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include "quicksearchline.h"
#include <Akonadi/KMime/MessageStatus>
#include <QIcon>
#include <QLineEdit>
class QStringListModel;
class QAction;
namespace MessageList
{
namespace Core
{
class FilterSavedMenu;
class Filter;
class MESSAGELIST_TESTS_EXPORT SearchLineStatus : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchLineStatus(QWidget *parent = nullptr);
    ~SearchLineStatus() override;

    void setLocked(bool b);
    Q_REQUIRED_RESULT bool locked() const;

    void setContainsOutboundMessages(bool containsOutboundMessages);
    Q_REQUIRED_RESULT bool containsOutboundMessages() const;
    Q_REQUIRED_RESULT QuickSearchLine::SearchOptions searchOptions() const;
    void setSearchOptions(QuickSearchLine::SearchOptions opts);
    void setFilterMessageStatus(const QVector<Akonadi::MessageStatus> &newLstStatus);
    void addCompletionItem(const QString &str);
    void slotClearHistory();
    void clearFilterButtonClicked();
Q_SIGNALS:
    void filterActionChanged(const QVector<Akonadi::MessageStatus> &lst);
    void searchOptionChanged();
    void clearButtonClicked();
    void forceLostFocus();
    void saveFilter();
    void activateFilter(MessageList::Core::Filter *f);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private Q_SLOTS:
    void slotToggledLockAction();
    void showMenu();
    void slotFilterActionClicked(QAction *act);

private:
    void clearFilterAction();
    void createFilterAction(const QIcon &icon, const QString &text, int value);
    void createMenuSearch();
    void updateLockAction();
    void initializeActions();
    void updateFilterActionIcon();
    void updateFilters();
    void createFilterByAction();
    void clearFilterByAction();
    void changeSearchAgainstFromOrToText();
    void slotClear();
    void showSavedFiltersMenu();
    void slotConfigureFilters();
    void slotActivateFilter(const QString &identifier);

    bool mLocked = false;
    bool mHasFilter = false;
    QIcon mWithoutFilter;
    QIcon mWithFilter;
    QAction *mLockAction = nullptr;
    QAction *mFiltersAction = nullptr;
    QAction *mSaveFilterAction = nullptr;
    QMenu *mFilterMenu = nullptr;
    QCompleter *mCompleter = nullptr;
    QList<QAction *> mFilterListActions;
    QAction *mSearchEveryWhereAction = nullptr;
    QAction *mSearchAgainstBodyAction = nullptr;
    QAction *mSearchAgainstSubjectAction = nullptr;
    QAction *mSearchAgainstFromOrToAction = nullptr;
    QAction *mSearchAgainstBccAction = nullptr;
    QStringListModel *mCompleterListModel = nullptr;
    FilterSavedMenu *mFilterSavedMenu = nullptr;
    QStringList mListCompetion;
    QString mColorName;
    bool mContainsOutboundMessages = false;
};
}
}

