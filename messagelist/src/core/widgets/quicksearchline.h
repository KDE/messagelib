/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messagelist_export.h"
#include <Akonadi/KMime/MessageStatus>
#include <QWidget>

class QComboBox;
class QToolButton;
namespace MessageList
{
namespace Core
{
class SearchLineStatus;
class Filter;
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

    enum SearchOption {
        SearchEveryWhere = 1,
        SearchAgainstBody = 2,
        SearchAgainstSubject = 4,
        SearchAgainstFrom = 8,
        SearchAgainstBcc = 16,
        SearchAgainstTo = 32
    };

    Q_ENUM(SearchOption)
    Q_DECLARE_FLAGS(SearchOptions, SearchOption)

    Q_REQUIRED_RESULT SearchOptions searchOptions() const;

    void focusQuickSearch(const QString &selectedText);

    Q_REQUIRED_RESULT QComboBox *tagFilterComboBox() const;
    Q_REQUIRED_RESULT SearchLineStatus *searchEdit() const;
    void resetFilter();
    Q_REQUIRED_RESULT QVector<Akonadi::MessageStatus> status() const;

    void updateComboboxVisibility();

    Q_REQUIRED_RESULT bool containsOutboundMessages() const;
    void setContainsOutboundMessages(bool containsOutboundMessages);

    void changeQuicksearchVisibility(bool show);
    void addCompletionItem(const QString &str);

    void setSearchOptions(QuickSearchLine::SearchOptions opts);
    void setFilterMessageStatus(const QVector<Akonadi::MessageStatus> &newLstStatus);

Q_SIGNALS:
    void clearButtonClicked();
    void searchEditTextEdited(const QString &);
    void searchOptionChanged();
    void statusButtonsClicked();
    void forceLostFocus();
    void saveFilter();
    void activateFilter(Filter *f);

protected:
    bool eventFilter(QObject *object, QEvent *e) override;
private Q_SLOTS:
    void slotSearchEditTextEdited(const QString &text);
    void slotClearButtonClicked();
    void slotFilterActionChanged(const QVector<Akonadi::MessageStatus> &lst);

private:
    SearchLineStatus *const mSearchEdit;
    QComboBox *const mTagFilterCombo;
    QVector<Akonadi::MessageStatus> mLstStatus;
};
}
}
