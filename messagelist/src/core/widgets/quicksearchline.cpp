/*
  SPDX-FileCopyrightText: 2014-2025 Laurent Montel <montel@kde.org>

 SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "quicksearchline.h"
#include "messagelistsettings.h"
#include <KStringHandler>

#include "core/filter.h"
#include "searchlinestatus.h"
#include "searchstatusbuttons.h"
#include <KLocalizedString>

#include "core/widgets/searchlinecommandwidget.h"
#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardPaths>
using namespace MessageList::Core;
QuickSearchLine::QuickSearchLine(QWidget *parent)
    : QWidget(parent)
    , mSearchEdit(new SearchLineStatus(this))
    , mSearchStatusButtons(new SearchStatusButtons(this))
    , mSearchMessageByButtons(new SearchMessageByButtons(this))
    , mSearchLineCommandWidget(new SearchLineCommandWidget(this))
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

    vbox->addWidget(mSearchMessageByButtons);
    mSearchMessageByButtons->setVisible(false);
    vbox->addWidget(mSearchLineCommandWidget);
    mSearchLineCommandWidget->setVisible(false);
    connect(mSearchEdit, &SearchLineStatus::searchCommandActionRequested, this, [this]() {
        mSearchLineCommandWidget->setVisible(!mSearchLineCommandWidget->isVisible());
    });
    connect(mSearchLineCommandWidget, &SearchLineCommandWidget::insertCommand, mSearchEdit, &SearchLineStatus::slotInsertCommand);

    connect(mSearchEdit, &QLineEdit::textChanged, this, [this](const QString &str) {
        MessageList::Core::SearchLineCommand c;
        c.parseSearchLineCommand(str);
        mSearchLineCommandWidget->setLabel(c.generateCommadLineStr());
    });

    connect(mSearchEdit, &SearchLineStatus::forceLostFocus, this, &QuickSearchLine::forceLostFocus);
    mSearchEdit->setPlaceholderText(i18nc("Search for messages.", "Search"));
    mSearchEdit->setObjectName(QLatin1StringView("quicksearch"));
    mSearchEdit->setClearButtonEnabled(true);
    connect(mSearchMessageByButtons, &SearchMessageByButtons::searchOptionChanged, this, [this]() {
        mSearchEdit->filterAdded();
        Q_EMIT searchOptionChanged();
    });

    connect(mSearchEdit, &QLineEdit::textChanged, this, &QuickSearchLine::slotSearchEditTextEdited);
    connect(mSearchEdit, &SearchLineStatus::clearButtonClicked, this, &QuickSearchLine::slotClearButtonClicked);
    connect(mSearchEdit, &SearchLineStatus::saveFilter, this, &QuickSearchLine::saveFilter);
    connect(mSearchEdit, &SearchLineStatus::activateFilter, this, &QuickSearchLine::activateFilter);

    hbox->addWidget(mSearchEdit);
    mSearchStatusButtons->setObjectName(QLatin1StringView("mSearchStatusButtons"));
    hbox->addWidget(mSearchStatusButtons);
    connect(mSearchStatusButtons, &SearchStatusButtons::filterStatusChanged, this, &QuickSearchLine::slotFilterActionChanged);

    // The status filter button. Will be populated later, as populateStatusFilterCombo() is virtual
    mTagFilterCombo->setObjectName(QLatin1StringView("mTagFilterCombo"));
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

QuickSearchLine::~QuickSearchLine() = default;

void QuickSearchLine::slotSearchEditTextEdited(const QString &text)
{
    int minimumStringLength = 3;
    if (text.startsWith(QLatin1Char('"')) && text.endsWith(QLatin1Char('"'))) {
        minimumStringLength = 5;
    }
    if (!text.trimmed().isEmpty()) {
        if (KStringHandler::logicalLength(text) >= minimumStringLength) {
            mSearchMessageByButtons->setVisible(true);
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
    mSearchStatusButtons->clearFilter();
    mSearchMessageByButtons->clearFilter();
    mSearchMessageByButtons->setVisible(false);
    Q_EMIT clearButtonClicked();
}

void QuickSearchLine::setSearchOptions(SearchMessageByButtons::SearchOptions opts)
{
    mSearchMessageByButtons->setSearchOptions(opts);
    mSearchMessageByButtons->setVisible(true);
    mSearchEdit->filterAdded();
}

SearchMessageByButtons::SearchOptions QuickSearchLine::searchOptions() const
{
    return mSearchMessageByButtons->searchOptions();
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
    mSearchStatusButtons->clearFilter();
}

void QuickSearchLine::slotFilterActionChanged(const QList<Akonadi::MessageStatus> &lst)
{
    mLstStatus = lst;
    mSearchEdit->filterAdded();
    Q_EMIT statusButtonsClicked();
}

void QuickSearchLine::setFilterMessageStatus(const QList<Akonadi::MessageStatus> &newLstStatus)
{
    mLstStatus = newLstStatus;
    mSearchEdit->filterAdded();
    mSearchStatusButtons->setFilterMessageStatus(mLstStatus);
}

QList<Akonadi::MessageStatus> QuickSearchLine::status() const
{
    return mLstStatus;
}

bool QuickSearchLine::containsOutboundMessages() const
{
    return mSearchMessageByButtons->containsOutboundMessages();
}

void QuickSearchLine::setContainsOutboundMessages(bool containsOutboundMessages)
{
    mSearchMessageByButtons->setContainsOutboundMessages(containsOutboundMessages);
}

void QuickSearchLine::updateComboboxVisibility()
{
    mTagFilterCombo->setVisible(!mSearchEdit->isHidden() && (mTagFilterCombo->count() > 0));
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
    mSearchStatusButtons->setVisible(show);
}

void QuickSearchLine::addCompletionItem(const QString &str)
{
    mSearchEdit->addCompletionItem(str);
}

#include "moc_quicksearchline.cpp"
