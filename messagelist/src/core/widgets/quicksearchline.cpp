/*
  Copyright (c) 2014-2017 Montel Laurent <montel@kde.org>

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

#include "quicksearchline.h"
#include "messagelistsettings.h"

#include <Akonadi/KMime/MessageStatus>

#include "searchlinestatus.h"
#include <KLocalizedString>
#include <KComboBox>

#include <QPushButton>

#include <QHBoxLayout>
#include <QStandardPaths>

using namespace MessageList::Core;
QuickSearchLine::QuickSearchLine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->setSpacing(0);

    QWidget *w = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    w->setLayout(hbox);
    vbox->addWidget(w);

    mSearchEdit = new SearchLineStatus(this);
    connect(mSearchEdit, &SearchLineStatus::filterActionChanged, this, &QuickSearchLine::slotFilterActionChanged);
    connect(mSearchEdit, &SearchLineStatus::searchOptionChanged, this, &QuickSearchLine::searchOptionChanged);
    mSearchEdit->setPlaceholderText(i18nc("Search for messages.", "Search"));
    mSearchEdit->setObjectName(QStringLiteral("quicksearch"));
    mSearchEdit->setClearButtonEnabled(true);

    connect(mSearchEdit, &QLineEdit::textChanged, this, &QuickSearchLine::slotSearchEditTextEdited);
    connect(mSearchEdit, &SearchLineStatus::clearButtonClicked, this, &QuickSearchLine::slotClearButtonClicked);

    hbox->addWidget(mSearchEdit);

    // The status filter button. Will be populated later, as populateStatusFilterCombo() is virtual
    mTagFilterCombo = new KComboBox(this);
    mTagFilterCombo->setMaximumWidth(300);
    mTagFilterCombo->setMaximumWidth(200);
    mTagFilterCombo->hide();
    hbox->addWidget(mTagFilterCombo);

    //Be disable until we have a storageModel => logical that it's disable.
    mSearchEdit->setEnabled(false);
    mTagFilterCombo->setEnabled(false);

    installEventFilter(this);
    mTagFilterCombo->installEventFilter(this);
    changeQuicksearchVisibility(MessageListSettings::self()->showQuickSearch());
}

QuickSearchLine::~QuickSearchLine()
{
}

void QuickSearchLine::slotSearchEditTextEdited(const QString &text)
{
    int minimumStringLength = 3;
    if (text.startsWith(QLatin1Char('"')) && text.endsWith(QLatin1Char('"'))) {
        minimumStringLength = 5;
    }
    if (!text.trimmed().isEmpty()) {
        if (text.length() >= minimumStringLength) {
            Q_EMIT searchEditTextEdited(text);
        }
    } else {
        slotClearButtonClicked();
    }
}

void QuickSearchLine::slotClearButtonClicked()
{
    if (mTagFilterCombo->isVisible()) {
        mTagFilterCombo->setCurrentIndex(0);
    }
    mSearchEdit->clearFilterButtonClicked();
    Q_EMIT clearButtonClicked();
}

QuickSearchLine::SearchOptions QuickSearchLine::searchOptions() const
{
    return mSearchEdit->searchOptions();
}

void QuickSearchLine::focusQuickSearch(const QString &selectedText)
{
    if (!selectedText.isEmpty()) {
        mSearchEdit->setText(selectedText);
    }
    mSearchEdit->setFocus();
}

KComboBox *QuickSearchLine::tagFilterComboBox() const
{
    return mTagFilterCombo;
}

SearchLineStatus *QuickSearchLine::searchEdit() const
{
    return mSearchEdit;
}

void QuickSearchLine::resetFilter()
{
    if (mTagFilterCombo->isVisible()) {
        mTagFilterCombo->setCurrentIndex(0);
    }
    mSearchEdit->clearFilterButtonClicked();
    mSearchEdit->setLocked(false);
}

void QuickSearchLine::slotFilterActionChanged(const QList<Akonadi::MessageStatus> &lst)
{
    mLstStatus = lst;
    Q_EMIT statusButtonsClicked();
}

QList<Akonadi::MessageStatus> QuickSearchLine::status() const
{
    return mLstStatus;
}

bool QuickSearchLine::containsOutboundMessages() const
{
    return mSearchEdit->containsOutboundMessages();
}

void QuickSearchLine::setContainsOutboundMessages(bool containsOutboundMessages)
{
    mSearchEdit->setContainsOutboundMessages(containsOutboundMessages);
}

void QuickSearchLine::updateComboboxVisibility()
{
    mTagFilterCombo->setVisible(!mSearchEdit->isHidden() && mTagFilterCombo->count());
}

bool QuickSearchLine::eventFilter(QObject *object, QEvent *e)
{
    const bool shortCutOverride = (e->type() == QEvent::ShortcutOverride);
    if (shortCutOverride) {
        e->accept();
        return true;
    }
    return QWidget::eventFilter(object, e);
}

void QuickSearchLine::changeQuicksearchVisibility(bool show)
{
    mSearchEdit->setVisible(show);
    mTagFilterCombo->setVisible(show && mTagFilterCombo->count());
}

void QuickSearchLine::addCompletionItem(const QString &str)
{
    mSearchEdit->addCompletionItem(str);
}
