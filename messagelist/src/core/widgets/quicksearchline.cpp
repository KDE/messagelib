/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

 SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "quicksearchline.h"
#include "messagelistsettings.h"
#include <KStringHandler>

#include "core/filter.h"
#include "searchlinestatus.h"
#include <KLocalizedString>

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardPaths>

using namespace MessageList::Core;
QuickSearchLine::QuickSearchLine(QWidget *parent)
    : QWidget(parent)
    , mSearchEdit(new SearchLineStatus(this))
    , mTagFilterCombo(new QComboBox(this))
{
    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins({});
    vbox->setSpacing(0);

    auto w = new QWidget;
    auto hbox = new QHBoxLayout(w);
    hbox->setContentsMargins({});
    hbox->setSpacing(0);
    vbox->addWidget(w);

    connect(mSearchEdit, &SearchLineStatus::filterActionChanged, this, &QuickSearchLine::slotFilterActionChanged);
    connect(mSearchEdit, &SearchLineStatus::searchOptionChanged, this, &QuickSearchLine::searchOptionChanged);
    connect(mSearchEdit, &SearchLineStatus::forceLostFocus, this, &QuickSearchLine::forceLostFocus);
    mSearchEdit->setPlaceholderText(i18nc("Search for messages.", "Search"));
    mSearchEdit->setObjectName(QStringLiteral("quicksearch"));
    mSearchEdit->setClearButtonEnabled(true);

    connect(mSearchEdit, &QLineEdit::textChanged, this, &QuickSearchLine::slotSearchEditTextEdited);
    connect(mSearchEdit, &SearchLineStatus::clearButtonClicked, this, &QuickSearchLine::slotClearButtonClicked);
    connect(mSearchEdit, &SearchLineStatus::saveFilter, this, &QuickSearchLine::saveFilter);
    connect(mSearchEdit, &SearchLineStatus::activateFilter, this, &QuickSearchLine::activateFilter);

    hbox->addWidget(mSearchEdit);

    // The status filter button. Will be populated later, as populateStatusFilterCombo() is virtual
    mTagFilterCombo->setMaximumWidth(300);
    mTagFilterCombo->setMaximumWidth(200);
    mTagFilterCombo->hide();
    hbox->addWidget(mTagFilterCombo);

    // Be disable until we have a storageModel => logical that it's disable.
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
        if (KStringHandler::logicalLength(text) >= minimumStringLength) {
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

void QuickSearchLine::setSearchOptions(QuickSearchLine::SearchOptions opts)
{
    mSearchEdit->setSearchOptions(opts);
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

QComboBox *QuickSearchLine::tagFilterComboBox() const
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

void QuickSearchLine::slotFilterActionChanged(const QVector<Akonadi::MessageStatus> &lst)
{
    mLstStatus = lst;
    Q_EMIT statusButtonsClicked();
}

void QuickSearchLine::setFilterMessageStatus(const QVector<Akonadi::MessageStatus> &newLstStatus)
{
    mLstStatus = newLstStatus;
    mSearchEdit->setFilterMessageStatus(mLstStatus);
}

QVector<Akonadi::MessageStatus> QuickSearchLine::status() const
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
