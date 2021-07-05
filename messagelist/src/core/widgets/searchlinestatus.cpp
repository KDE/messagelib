/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinestatus.h"
#include "configurefiltersdialog.h"
#include "core/filtersavedmanager.h"
#include "filternamedialog.h"
#include "filtersavedmenu.h"
#include "messagelist_debug.h"

#include <KColorScheme>
#include <KIconEngine>
#include <KIconLoader>
#include <KLocalizedString>
#include <QAbstractItemView>
#include <QAction>
#include <QCompleter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QPointer>
#include <QPushButton>
#include <QStandardPaths>
#include <QStringListModel>
#include <QWidgetAction>

static const char qLineEditclearButtonActionNameC[] = "_q_qlineeditclearaction";
#define MAX_COMPLETION_ITEMS 20
using namespace MessageList::Core;
SearchLineStatus::SearchLineStatus(QWidget *parent)
    : QLineEdit(parent)
{
    mCompleter = new QCompleter(this);
    mCompleterListModel = new QStringListModel(this);
    mCompleter->setModel(mCompleterListModel);
    setCompleter(mCompleter);

    setClearButtonEnabled(true);
    initializeActions();
    createMenuSearch();
    auto act = findChild<QAction *>(QLatin1String(qLineEditclearButtonActionNameC));
    if (act) {
        connect(act, &QAction::triggered, this, &SearchLineStatus::slotClear);
    } else {
        qCWarning(MESSAGELIST_LOG) << "Clear button name was changed ! Please verify qt code";
    }
    connect(FilterSavedManager::self(), &FilterSavedManager::activateFilter, this, &SearchLineStatus::slotActivateFilter);
}

SearchLineStatus::~SearchLineStatus()
{
}

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
    mLockAction = addAction(QIcon::fromTheme(QStringLiteral("object-locked")), QLineEdit::TrailingPosition);
    mLockAction->setWhatsThis(i18nc("@info:whatsthis",
                                    "Toggle this button if you want to keep your quick search "
                                    "locked when moving to other folders or when narrowing the search "
                                    "by message status."));

    connect(mLockAction, &QAction::triggered, this, &SearchLineStatus::slotToggledLockAction);
    updateLockAction();

    const QStringList overlays = QStringList() << QStringLiteral("list-add");
    mWithFilter = QIcon(new KIconEngine(QStringLiteral("view-filter"), KIconLoader::global(), overlays));
    mWithoutFilter = QIcon::fromTheme(QStringLiteral("view-filter"));
    mFiltersAction = addAction(mWithoutFilter, QLineEdit::LeadingPosition);
    mFiltersAction->setToolTip(i18n("Filter Mails by Status"));
    connect(mFiltersAction, &QAction::triggered, this, &SearchLineStatus::showMenu);

    mSaveFilterAction = addAction(QIcon::fromTheme(QStringLiteral("edit-find")), QLineEdit::LeadingPosition);
    mSaveFilterAction->setToolTip(i18n("Saved Filter"));
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

void SearchLineStatus::updateFilters()
{
    QVector<Akonadi::MessageStatus> lstStatus;

    for (QAction *act : std::as_const(mFilterListActions)) {
        if (act->isChecked()) {
            Akonadi::MessageStatus status;
            status.fromQInt32(static_cast<qint32>(act->data().toInt()));
            lstStatus.append(status);
        }
    }
    mHasFilter = !lstStatus.isEmpty();
    Q_EMIT filterActionChanged(lstStatus);
    updateFilterActionIcon();
}

void SearchLineStatus::showSavedFiltersMenu()
{
    mFilterSavedMenu->exec(mapToGlobal(QPoint(0, height())));
}

void SearchLineStatus::showMenu()
{
    if (mFilterMenu->exec(mapToGlobal(QPoint(0, height())))) {
        updateFilters();
    }
}

void SearchLineStatus::clearFilterAction()
{
    for (QAction *act : std::as_const(mFilterListActions)) {
        act->setChecked(false);
    }
    mHasFilter = false;
    updateFilterActionIcon();
}

void SearchLineStatus::createFilterAction(const QIcon &icon, const QString &text, int value)
{
    auto act = new QAction(icon, text, this);
    act->setCheckable(true);
    act->setData(value);
    mFilterMenu->addAction(act);
    mFilterListActions.append(act);
}

void SearchLineStatus::updateFilterActionIcon()
{
    mFiltersAction->setIcon(mHasFilter ? mWithFilter : mWithoutFilter);
    if (mColorName.isEmpty()) {
        const KColorScheme::BackgroundRole bgColorScheme(KColorScheme::PositiveBackground);
        KStatefulBrush bgBrush(KColorScheme::View, bgColorScheme);
        mColorName = bgBrush.brush(palette()).color().name();
    }
    setStyleSheet(mHasFilter ? QStringLiteral("QLineEdit{ background-color:%1 }").arg(mColorName) : QString());
}

void SearchLineStatus::clearFilterButtonClicked()
{
    clearFilterAction();
    clearFilterByAction();
    updateFilters();
}

void SearchLineStatus::createMenuSearch()
{
    mFilterMenu = new QMenu(this);
    mFilterMenu->setObjectName(QStringLiteral("filtermenu"));
    auto clearWidgetAction = new QWidgetAction(mFilterMenu);
    auto clearFilterButton = new QPushButton(i18n("Clear Filter"), mFilterMenu);
    connect(clearFilterButton, &QPushButton::clicked, this, &SearchLineStatus::clearFilterButtonClicked);

    clearWidgetAction->setDefaultWidget(clearFilterButton);
    mFilterMenu->addAction(clearWidgetAction);
    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-unread")),
                       i18nc("@action:inmenu Status of a message", "Unread"),
                       Akonadi::MessageStatus::statusUnread().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-replied")),
                       i18nc("@action:inmenu Status of a message", "Replied"),
                       Akonadi::MessageStatus::statusReplied().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-forwarded")),
                       i18nc("@action:inmenu Status of a message", "Forwarded"),
                       Akonadi::MessageStatus::statusForwarded().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-mark-important")),
                       i18nc("@action:inmenu Status of a message", "Important"),
                       Akonadi::MessageStatus::statusImportant().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-task")),
                       i18nc("@action:inmenu Status of a message", "Action Item"),
                       Akonadi::MessageStatus::statusToAct().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-thread-watch")),
                       i18nc("@action:inmenu Status of a message", "Watched"),
                       Akonadi::MessageStatus::statusWatched().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-thread-ignored")),
                       i18nc("@action:inmenu Status of a message", "Ignored"),
                       Akonadi::MessageStatus::statusIgnored().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-attachment")),
                       i18nc("@action:inmenu Status of a message", "Has Attachment"),
                       Akonadi::MessageStatus::statusHasAttachment().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-invitation")),
                       i18nc("@action:inmenu Status of a message", "Has Invitation"),
                       Akonadi::MessageStatus::statusHasInvitation().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-mark-junk")),
                       i18nc("@action:inmenu Status of a message", "Spam"),
                       Akonadi::MessageStatus::statusSpam().toQInt32());

    createFilterAction(QIcon::fromTheme(QStringLiteral("mail-mark-notjunk")),
                       i18nc("@action:inmenu Status of a message", "Ham"),
                       Akonadi::MessageStatus::statusHam().toQInt32());
    createFilterByAction();
}

void SearchLineStatus::createFilterByAction()
{
    mFilterMenu->addSeparator();
    auto grp = new QActionGroup(mFilterMenu);

    mSearchEveryWhereAction = new QAction(i18n("Full Message"), mFilterMenu);
    mSearchEveryWhereAction->setCheckable(true);
    mSearchEveryWhereAction->setChecked(true);

    mFilterMenu->addAction(mSearchEveryWhereAction);
    grp->addAction(mSearchEveryWhereAction);

    mSearchAgainstBodyAction = new QAction(i18n("Body"), mFilterMenu);
    grp->addAction(mSearchAgainstBodyAction);
    mFilterMenu->addAction(mSearchAgainstBodyAction);
    mSearchAgainstBodyAction->setCheckable(true);

    mSearchAgainstSubjectAction = new QAction(i18n("Subject"), mFilterMenu);
    grp->addAction(mSearchAgainstSubjectAction);
    mFilterMenu->addAction(mSearchAgainstSubjectAction);
    mSearchAgainstSubjectAction->setCheckable(true);

    mSearchAgainstFromOrToAction = new QAction(mFilterMenu);
    changeSearchAgainstFromOrToText();
    grp->addAction(mSearchAgainstFromOrToAction);
    mFilterMenu->addAction(mSearchAgainstFromOrToAction);
    mSearchAgainstFromOrToAction->setCheckable(true);

    mSearchAgainstBccAction = new QAction(i18n("BCC"), mFilterMenu);
    grp->addAction(mSearchAgainstBccAction);
    mFilterMenu->addAction(mSearchAgainstBccAction);
    mSearchAgainstBccAction->setCheckable(true);
    connect(grp, &QActionGroup::triggered, this, &SearchLineStatus::slotFilterActionClicked);
}

void SearchLineStatus::clearFilterByAction()
{
    mSearchEveryWhereAction->setChecked(true);
}

bool SearchLineStatus::containsOutboundMessages() const
{
    return mContainsOutboundMessages;
}

void SearchLineStatus::setContainsOutboundMessages(bool containsOutboundMessages)
{
    if (mContainsOutboundMessages != containsOutboundMessages) {
        mContainsOutboundMessages = containsOutboundMessages;
        changeSearchAgainstFromOrToText();
    }
}

void SearchLineStatus::changeSearchAgainstFromOrToText()
{
    if (mContainsOutboundMessages) {
        mSearchAgainstFromOrToAction->setText(i18n("To"));
    } else {
        mSearchAgainstFromOrToAction->setText(i18n("From"));
    }
}

QuickSearchLine::SearchOptions SearchLineStatus::searchOptions() const
{
    QuickSearchLine::SearchOptions searchOptions;
    if (mSearchEveryWhereAction->isChecked()) {
        searchOptions |= QuickSearchLine::SearchEveryWhere;
    }
    if (mSearchAgainstBodyAction->isChecked()) {
        searchOptions |= QuickSearchLine::SearchAgainstBody;
    }
    if (mSearchAgainstSubjectAction->isChecked()) {
        searchOptions |= QuickSearchLine::SearchAgainstSubject;
    }
    if (mSearchAgainstFromOrToAction->isChecked()) {
        if (mContainsOutboundMessages) {
            searchOptions |= QuickSearchLine::SearchAgainstTo;
        } else {
            searchOptions |= QuickSearchLine::SearchAgainstFrom;
        }
    }
    if (mSearchAgainstBccAction->isChecked()) {
        searchOptions |= QuickSearchLine::SearchAgainstBcc;
    }
    return searchOptions;
}

void SearchLineStatus::setSearchOptions(QuickSearchLine::SearchOptions opts)
{
    mSearchEveryWhereAction->setChecked(opts & QuickSearchLine::SearchEveryWhere);
    mSearchAgainstBodyAction->setChecked(opts & QuickSearchLine::SearchAgainstBody);
    mSearchAgainstSubjectAction->setChecked(opts & QuickSearchLine::SearchAgainstSubject);
    mSearchAgainstBccAction->setChecked(opts & QuickSearchLine::SearchAgainstBcc);
    if (mContainsOutboundMessages) {
        mSearchAgainstFromOrToAction->setChecked(opts & QuickSearchLine::SearchAgainstTo);
    } else {
        mSearchAgainstFromOrToAction->setChecked(opts & QuickSearchLine::SearchAgainstFrom);
    }
}

void SearchLineStatus::setFilterMessageStatus(const QVector<Akonadi::MessageStatus> &newLstStatus)
{
    clearFilterAction();
    clearFilterByAction();
    // TODO unchecked all checkbox
    for (const Akonadi::MessageStatus &status : newLstStatus) {
        for (QAction *act : std::as_const(mFilterListActions)) {
            if (static_cast<qint32>(act->data().toInt()) == status.toQInt32()) {
                act->setChecked(true);
                mHasFilter = true;
                break;
            }
        }
    }
    updateFilterActionIcon();
}

void SearchLineStatus::slotFilterActionClicked(QAction *act)
{
    Q_UNUSED(act)
    Q_EMIT searchOptionChanged();
}

void SearchLineStatus::addCompletionItem(const QString &str)
{
    mListCompetion.removeAll(str);
    mListCompetion.prepend(str);
    while (mListCompetion.size() > MAX_COMPLETION_ITEMS) {
        mListCompetion.removeLast();
    }
    mCompleterListModel->setStringList(mListCompetion);
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
    mCompleterListModel->setStringList(mListCompetion);
}
