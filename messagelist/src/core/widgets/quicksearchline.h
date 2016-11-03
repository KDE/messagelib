/*
  Copyright (c) 2014-2016 Montel Laurent <montel@kde.org>

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

class KComboBox;
class QToolButton;
namespace MessageList
{
namespace Core
{
class SearchLineStatus;
class MESSAGELIST_EXPORT QuickSearchLine : public QWidget
{
    Q_OBJECT
public:
    explicit QuickSearchLine(QWidget *parent = Q_NULLPTR);
    ~QuickSearchLine();

    enum SearchOption {
        SearchEveryWhere = 1,
        SearchAgainstBody = 2,
        SearchAgainstSubject = 4,
        SearchAgainstFrom = 8,
        SearchAgainstBcc = 16,
        SearchAgainstTo = 32
    };

    Q_ENUMS(SearchOption)
    Q_DECLARE_FLAGS(SearchOptions, SearchOption)

    SearchOptions searchOptions() const;

    void focusQuickSearch(const QString &selectedText);

    KComboBox *tagFilterComboBox() const;
    SearchLineStatus *searchEdit() const;
    QToolButton *openFullSearchButton() const;
    void resetFilter();
    QList<Akonadi::MessageStatus> status() const;

    void updateComboboxVisibility();

    bool containsOutboundMessages() const;
    void setContainsOutboundMessages(bool containsOutboundMessages);

    void changeQuicksearchVisibility(bool show);

Q_SIGNALS:
    void clearButtonClicked();
    void searchEditTextEdited(const QString &);
    void searchOptionChanged();
    void statusButtonsClicked();

protected:
    bool eventFilter(QObject *object, QEvent *e) Q_DECL_OVERRIDE;
private Q_SLOTS:
    void slotSearchEditTextEdited(const QString &text);
    void slotClearButtonClicked();
    void slotFilterActionChanged(const QList<Akonadi::MessageStatus> &lst);
private:
    SearchLineStatus *mSearchEdit;
    KComboBox *mTagFilterCombo;
    QList<Akonadi::MessageStatus> mLstStatus;
};
}
}
#endif // QUICKSEARCHLINE_H
