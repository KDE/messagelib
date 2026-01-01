/*
  SPDX-FileCopyrightText: 2014-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "config-messagelist.h"
#include "messagelist_export.h"
#include "searchmessagebybuttons.h"
#include <Akonadi/MessageStatus>
#include <QWidget>

class QComboBox;
namespace MessageList
{
namespace Core
{
class SearchLineStatus;
class Filter;
class SearchStatusButtons;
class SearchMessageByButtons;
class SearchLineCommandWidget;
/**
 * @brief The QuickSearchLine class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGELIST_EXPORT QuickSearchLine : public QWidget
{
    Q_OBJECT
public:
    explicit QuickSearchLine(QWidget *parent = nullptr);
    ~QuickSearchLine() override;

    [[nodiscard]] SearchMessageByButtons::SearchOptions searchOptions() const;

    void focusQuickSearch(const QString &selectedText);

    [[nodiscard]] QComboBox *tagFilterComboBox() const;
    [[nodiscard]] SearchLineStatus *searchEdit() const;
    void resetFilter();
    [[nodiscard]] QList<Akonadi::MessageStatus> status() const;

    void updateComboboxVisibility();

    [[nodiscard]] bool containsOutboundMessages() const;
    void setContainsOutboundMessages(bool containsOutboundMessages);

    void changeQuicksearchVisibility(bool show);
    void addCompletionItem(const QString &str);

    void setSearchOptions(SearchMessageByButtons::SearchOptions opts);
    void setFilterMessageStatus(const QList<Akonadi::MessageStatus> &newLstStatus);

Q_SIGNALS:
    void clearButtonClicked();
    void searchEditTextEdited(const QString &);
    void searchOptionChanged();
    void statusButtonsClicked();
    void forceLostFocus();
    void saveFilter();
    void activateFilter(MessageList::Core::Filter *f);

protected:
    bool eventFilter(QObject *object, QEvent *e) override;

private:
    MESSAGELIST_NO_EXPORT void slotSearchEditTextEdited(const QString &text);
    MESSAGELIST_NO_EXPORT void slotClearButtonClicked();
    MESSAGELIST_NO_EXPORT void slotFilterActionChanged(const QList<Akonadi::MessageStatus> &lst);
    SearchLineStatus *const mSearchEdit;
    SearchStatusButtons *const mSearchStatusButtons;
    SearchMessageByButtons *const mSearchMessageByButtons;
    SearchLineCommandWidget *const mSearchLineCommandWidget;
    QComboBox *const mTagFilterCombo;
    QList<Akonadi::MessageStatus> mLstStatus;
};
}
}
