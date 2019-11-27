/*
  Copyright (c) 2014-2019 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#ifndef QUICKSEARCHLINE_H
#define QUICKSEARCHLINE_H

#include <QWidget>
#include "messagelist_export.h"
#include <Akonadi/KMime/MessageStatus>

class QComboBox;
class QToolButton;
namespace MessageList {
namespace Core {
class SearchLineStatus;
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
    Q_REQUIRED_RESULT QToolButton *openFullSearchButton() const;
    void resetFilter();
    Q_REQUIRED_RESULT QList<Akonadi::MessageStatus> status() const;

    void updateComboboxVisibility();

    Q_REQUIRED_RESULT bool containsOutboundMessages() const;
    void setContainsOutboundMessages(bool containsOutboundMessages);

    void changeQuicksearchVisibility(bool show);
    void addCompletionItem(const QString &str);

Q_SIGNALS:
    void clearButtonClicked();
    void searchEditTextEdited(const QString &);
    void searchOptionChanged();
    void statusButtonsClicked();
    void forceLostFocus();

protected:
    bool eventFilter(QObject *object, QEvent *e) override;
private Q_SLOTS:
    void slotSearchEditTextEdited(const QString &text);
    void slotClearButtonClicked();
    void slotFilterActionChanged(const QList<Akonadi::MessageStatus> &lst);
private:
    SearchLineStatus *mSearchEdit = nullptr;
    QComboBox *mTagFilterCombo = nullptr;
    QList<Akonadi::MessageStatus> mLstStatus;
};
}
}
#endif // QUICKSEARCHLINE_H
