/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "config-messagelist.h"
#include "messagelist_private_export.h"
#include "quicksearchline.h"
#include <Akonadi/MessageStatus>
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
class SearchLineCommandWidget;
class MESSAGELIST_TESTS_EXPORT SearchLineStatus : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchLineStatus(QWidget *parent = nullptr);
    ~SearchLineStatus() override;

    void setLocked(bool b);
    [[nodiscard]] bool locked() const;

    void addCompletionItem(const QString &str);
    void slotClearHistory();
    void clearFilterButtonClicked();
    void filterAdded();
Q_SIGNALS:
    void clearButtonClicked();
    void forceLostFocus();
    void saveFilter();
    void activateFilter(MessageList::Core::Filter *f);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    MESSAGELIST_NO_EXPORT void slotToggledLockAction();
    MESSAGELIST_NO_EXPORT void clearFilterAction();
    MESSAGELIST_NO_EXPORT void updateLockAction();
    MESSAGELIST_NO_EXPORT void initializeActions();
    MESSAGELIST_NO_EXPORT void updateFilterLineEditBackgroundColor();
    MESSAGELIST_NO_EXPORT void slotClear();
    MESSAGELIST_NO_EXPORT void showSavedFiltersMenu();
    MESSAGELIST_NO_EXPORT void slotConfigureFilters();
    MESSAGELIST_NO_EXPORT void slotActivateFilter(const QString &identifier);
    MESSAGELIST_NO_EXPORT void slotToggledChangeVisibleCommandWidgetAction();
    MESSAGELIST_NO_EXPORT void slotInsertCommand(const QString &command);

    bool mLocked = false;
    bool mHasFilter = false;
    QAction *mLockAction = nullptr;
    QAction *mSaveFilterAction = nullptr;
    QAction *mSearchCommandAction = nullptr;
    QCompleter *const mCompleter;
    QStringListModel *const mCompleterListModel;
    FilterSavedMenu *mFilterSavedMenu = nullptr;
    QStringList mListCompetion;
    QString mColorName;
#if USE_SEARCH_COMMAND_LINE
    SearchLineCommandWidget *mSearchLineCommandWidget = nullptr;
#endif
};
}
}
