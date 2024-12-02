/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_export.h"
#include <Akonadi/MessageStatus>
#include <QWidget>
class QHBoxLayout;
class QButtonGroup;
class QToolButton;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_EXPORT SearchMessageByButtons : public QWidget
{
    Q_OBJECT
public:
    enum SearchOption : uint8_t {
        SearchEveryWhere = 1,
        SearchAgainstBody = 2,
        SearchAgainstSubject = 4,
        SearchAgainstFrom = 8,
        SearchAgainstBcc = 16,
        SearchAgainstTo = 32,
        SearchAgainstCc = 64,
    };
    Q_ENUM(SearchOption)
    Q_DECLARE_FLAGS(SearchOptions, SearchOption)
    explicit SearchMessageByButtons(QWidget *parent = nullptr);
    ~SearchMessageByButtons() override;

    void changeSearchAgainstFromOrToText();

    void setContainsOutboundMessages(bool containsOutboundMessages);
    [[nodiscard]] bool containsOutboundMessages() const;

    [[nodiscard]] SearchMessageByButtons::SearchOptions searchOptions() const;
    void setSearchOptions(SearchMessageByButtons::SearchOptions opts);

    void clearFilter();
Q_SIGNALS:
    void searchOptionChanged();

private:
    MESSAGELIST_NO_EXPORT QToolButton *createFilterButton(const QString &text, SearchOption option);
    MESSAGELIST_NO_EXPORT void createButtons();
    QHBoxLayout *const mMainLayout;
    QButtonGroup *const mButtonGroup;
    QToolButton *mSearchAgainstFromOrToButton = nullptr;
    bool mContainsOutboundMessages = false;
};
}
}
