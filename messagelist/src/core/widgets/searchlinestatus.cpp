/*
  SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinestatus.h"
#include "searchlinecommand.h"

#include "configurefiltersdialog.h"
#include "core/filtersavedmanager.h"
#include "filtersavedmenu.h"
#include "messagelist_debug.h"
#include <KStatefulBrush>

#include <KColorScheme>
#include <KLocalizedString>
#include <QAbstractItemView>
#include <QAction>
#include <QActionGroup>
#include <QCompleter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QStringListModel>
#include <QWidgetAction>

static const char qLineEditclearButtonActionNameC[] = "_q_qlineeditclearaction";
#define MAX_COMPLETION_ITEMS 20
using namespace MessageList::Core;
SearchLineStatus::SearchLineStatus(QWidget *parent)
    : QLineEdit(parent)
    , mCompleter(new QCompleter(this))
    , mCompleterListModel(new QStringListModel(this))
{
    setProperty("_breeze_borders_sides", QVariant::fromValue(QFlags{Qt::BottomEdge}));
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setModel(mCompleterListModel);
    setCompleter(mCompleter);

    setClearButtonEnabled(true);
    initializeActions();
    auto act = findChild<QAction *>(QLatin1StringView(qLineEditclearButtonActionNameC));
    if (act) {
        connect(act, &QAction::triggered, this, &SearchLineStatus::slotClear);
    } else {
        qCWarning(MESSAGELIST_LOG) << "Clear button name was changed ! Please verify qt code";
    }
    connect(FilterSavedManager::self(), &FilterSavedManager::activateFilter, this, &SearchLineStatus::slotActivateFilter);
    loadSearchLineCommand();
}

SearchLineStatus::~SearchLineStatus() = default;

void SearchLineStatus::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        if (mCompleter->popup()->isVisible()) {
            QLineEdit::keyPressEvent(e);
        } else {
            Q_EMIT forceLostFocus();
        }
    } else if (e->key() == Qt::Key_Q && (e->modifiers().testFlag(Qt::ShiftModifier) && e->modifiers().testFlag(Qt::AltModifier))) {
        mLockAction->trigger();
    } else {
        QLineEdit::keyPressEvent(e);
    }
}

void SearchLineStatus::slotClear()
{
    Q_EMIT clearButtonClicked();
}

void SearchLineStatus::updateLockAction()
{
    if (mLocked) {
        mLockAction->setIcon(QIcon::fromTheme(QStringLiteral("object-locked")));
        mLockAction->setToolTip(i18nc("@info:tooltip", "Prevent the quick search field from being cleared when changing folders"));
    } else {
        mLockAction->setIcon(QIcon::fromTheme(QStringLiteral("object-unlocked")));
        mLockAction->setToolTip(i18nc("@info:tooltip", "Clear the quick search field when changing folders"));
    }
}

void SearchLineStatus::setLocked(bool b)
{
    if (mLocked != b) {
        slotToggledLockAction();
    }
}

bool SearchLineStatus::locked() const
{
    return mLocked;
}

void SearchLineStatus::initializeActions()
{
    mSearchCommandAction = addAction(QIcon::fromTheme(QStringLiteral("settings-configure")), QLineEdit::TrailingPosition);
    mSearchCommandAction->setWhatsThis(i18nc("@info:whatsthis", "Toggle this button if you want show or hide search command line widget."));
    connect(mSearchCommandAction, &QAction::triggered, this, &SearchLineStatus::searchCommandActionRequested);
    mLockAction = addAction(QIcon::fromTheme(QStringLiteral("object-locked")), QLineEdit::TrailingPosition);
    mLockAction->setWhatsThis(i18nc("@info:whatsthis",
                                    "Toggle this button if you want to keep your quick search "
                                    "locked when moving to other folders or when narrowing the search "
                                    "by message status."));

    connect(mLockAction, &QAction::triggered, this, &SearchLineStatus::slotToggledLockAction);
    updateLockAction();

    mSaveFilterAction = addAction(QIcon::fromTheme(QStringLiteral("edit-find")), QLineEdit::LeadingPosition);
    mSaveFilterAction->setToolTip(i18nc("@info:tooltip", "Saved Filter"));
    mFilterSavedMenu = new FilterSavedMenu(this);
    mSaveFilterAction->setMenu(mFilterSavedMenu);
    connect(mSaveFilterAction, &QAction::triggered, this, &SearchLineStatus::showSavedFiltersMenu);
    connect(mFilterSavedMenu, &FilterSavedMenu::saveFilter, this, &SearchLineStatus::saveFilter);
    connect(mFilterSavedMenu, &FilterSavedMenu::configureFilters, this, &SearchLineStatus::slotConfigureFilters);
}

void SearchLineStatus::slotActivateFilter(const QString &identifier)
{
    Filter *f = FilterSavedManager::self()->loadFilter(identifier);
    if (f) {
        Q_EMIT activateFilter(f);
    } else {
        qCWarning(MESSAGELIST_LOG) << "Impossible to load Filter from identifier :" << identifier;
    }
}

void SearchLineStatus::slotConfigureFilters()
{
    ConfigureFiltersDialog dlg(this);
    dlg.exec();
}

void SearchLineStatus::slotToggledLockAction()
{
    mLocked = !mLocked;
    updateLockAction();
}

void SearchLineStatus::showSavedFiltersMenu()
{
    mFilterSavedMenu->exec(mapToGlobal(QPoint(0, height())));
}

void SearchLineStatus::clearFilterAction()
{
    mHasFilter = false;
    updateFilterLineEditBackgroundColor();
}

void SearchLineStatus::updateFilterLineEditBackgroundColor()
{
    if (mColorName.isEmpty()) {
        const KColorScheme::BackgroundRole bgColorScheme(KColorScheme::PositiveBackground);
        KStatefulBrush bgBrush(KColorScheme::View, bgColorScheme);
        mColorName = bgBrush.brush(palette()).color().name();
    }
    setStyleSheet(mHasFilter ? QStringLiteral("QLineEdit{ background-color:%1 }").arg(mColorName) : QString());
}

void SearchLineStatus::clearFilterButtonClicked()
{
    mHasFilter = false;
    updateFilterLineEditBackgroundColor();
}

void SearchLineStatus::filterAdded()
{
    mHasFilter = true;
    updateFilterLineEditBackgroundColor();
}

void SearchLineStatus::addCompletionItem(const QString &str)
{
    mListCompetion.removeAll(str);
    mListCompetion.prepend(str);
    while (mListCompetion.size() > MAX_COMPLETION_ITEMS) {
        mListCompetion.removeLast();
    }
    mCompleterListModel->setStringList(mListCompetion + mCommandLineListCompletion);
}

void SearchLineStatus::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *popup = QLineEdit::createStandardContextMenu();
    if (popup) {
        popup->addSeparator();
        popup->addAction(QIcon::fromTheme(QStringLiteral("edit-clear-locationbar-rtl")), i18n("Clear History"), this, &SearchLineStatus::slotClearHistory);
        popup->exec(e->globalPos());
        delete popup;
    }
}

void SearchLineStatus::slotClearHistory()
{
    mListCompetion.clear();
    mCompleterListModel->setStringList(mCommandLineListCompletion);
}

void SearchLineStatus::slotInsertCommand(const QString &command)
{
    if (!text().isEmpty() && text().back() != QLatin1Char(' ')) {
        insert(QStringLiteral(" "));
    }
    insert(command);
}

void SearchLineStatus::loadSearchLineCommand()
{
    for (int i = SearchLineCommand::SearchLineType::To; i <= SearchLineCommand::SearchLineType::Category; ++i) {
        mCommandLineListCompletion.append(SearchLineCommand::generateCommandText(static_cast<SearchLineCommand::SearchLineType>(i)));
    }
}

#include "moc_searchlinestatus.cpp"
