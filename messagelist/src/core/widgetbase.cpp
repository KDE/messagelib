/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/widgetbase.h"
#include "core/aggregation.h"
#include "core/filter.h"
#include "core/filtersavedmanager.h"
#include "core/manager.h"
#include "core/messageitem.h"
#include "core/model.h"
#include "core/optionset.h"
#include "core/storagemodelbase.h"
#include "core/theme.h"
#include "core/view.h"
#include "core/widgets/quicksearchwarning.h"
#include "core/widgets/searchcollectionindexingwarning.h"
#include "core/widgets/tablockedwarning.h"
#include "messagelistsettings.h"
#include "widgets/searchlinestatus.h"

#include "utils/configureaggregationsdialog.h"
#include "utils/configurethemesdialog.h"

#include <QActionGroup>
#include <QHeaderView>
#include <QPointer>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>

#include "messagelist_debug.h"
#include <KLocalizedString>
#include <KMessageBox>
#include <QAction>
#include <QComboBox>
#include <QMenu>

#include <QUrl>

#include <Akonadi/KMime/MessageStatus>
#include <collection.h>
#include <core/widgets/filternamedialog.h>

using namespace MessageList::Core;

class Widget::Private
{
public:
    Private(Widget *owner)
        : q(owner)
    {
    }

    /**
     * Small helper for switching SortOrder::MessageSorting and SortOrder::SortDirection
     * on the fly.
     * After doing this, the sort indicator in the header is updated.
     */
    void switchMessageSorting(SortOrder::MessageSorting messageSorting, SortOrder::SortDirection sortDirection, int logicalHeaderColumnIndex);

    /**
     * Check if our sort order can still be used with this aggregation.
     * This can happen if the global aggregation changed, for example we can now
     * have "most recent in subtree" sorting with an aggregation without threading.
     * If this happens, reset to the default sort order and don't use the global sort
     * order.
     */
    void checkSortOrder(const StorageModel *storageModel);

    void setDefaultAggregationForStorageModel(const StorageModel *storageModel);
    void setDefaultThemeForStorageModel(const StorageModel *storageModel);
    void setDefaultSortOrderForStorageModel(const StorageModel *storageModel);
    void applyFilter();

    Widget *const q;

    QuickSearchWarning *quickSearchWarning = nullptr;
    SearchCollectionIndexingWarning *searchCollectionIndexingWarning = nullptr;
    TabLockedWarning *tabLockedWarning = nullptr;
    QuickSearchLine *quickSearchLine = nullptr;
    View *mView = nullptr;
    QString mLastAggregationId;
    QString mLastThemeId;
    QTimer *mSearchTimer = nullptr;
    StorageModel *mStorageModel = nullptr; ///< The currently displayed storage. The storage itself
    ///  is owned by MessageList::Widget.
    Aggregation *mAggregation = nullptr; ///< The currently set aggregation mode, a deep copy
    Theme *mTheme = nullptr; ///< The currently set theme, a deep copy
    SortOrder mSortOrder; ///< The currently set sort order
    Filter *mFilter = nullptr; ///< The currently applied filter, owned by us.
    bool mStorageUsesPrivateTheme = false; ///< true if the current folder does not use the global theme
    bool mStorageUsesPrivateAggregation = false; ///< true if the current folder does not use the global aggregation
    bool mStorageUsesPrivateSortOrder = false; ///< true if the current folder does not use the global sort order
    Akonadi::Collection mCurrentFolder; ///< The current folder
    int mCurrentStatusFilterIndex = 0;
    bool mStatusFilterComboPopulationInProgress = false;
    bool mLockTab = false;
};

Widget::Widget(QWidget *pParent)
    : QWidget(pParent)
    , d(new Private(this))
{
    Manager::registerWidget(this);
    connect(Manager::instance(), &Manager::aggregationsChanged, this, &Widget::aggregationsChanged);
    connect(Manager::instance(), &Manager::themesChanged, this, &Widget::themesChanged);

    setAutoFillBackground(true);
    setObjectName(QStringLiteral("messagelistwidget"));

    auto g = new QVBoxLayout(this);
    g->setContentsMargins({});
    g->setSpacing(0);

    d->quickSearchLine = new QuickSearchLine;
    d->quickSearchLine->setObjectName(QStringLiteral("quicksearchline"));
    connect(d->quickSearchLine, &QuickSearchLine::clearButtonClicked, this, &Widget::searchEditClearButtonClicked);

    connect(d->quickSearchLine, &QuickSearchLine::searchEditTextEdited, this, &Widget::searchEditTextEdited);
    connect(d->quickSearchLine, &QuickSearchLine::searchOptionChanged, this, &Widget::searchEditTextEdited);
    connect(d->quickSearchLine, &QuickSearchLine::statusButtonsClicked, this, &Widget::slotStatusButtonsClicked);
    connect(d->quickSearchLine, &QuickSearchLine::forceLostFocus, this, &Widget::forceLostFocus);
    connect(d->quickSearchLine, &QuickSearchLine::saveFilter, this, &Widget::slotSaveFilter);
    connect(d->quickSearchLine, &QuickSearchLine::activateFilter, this, &Widget::slotActivateFilter);
    g->addWidget(d->quickSearchLine, 0);
    d->quickSearchWarning = new QuickSearchWarning(this);
    g->addWidget(d->quickSearchWarning, 0);
    d->searchCollectionIndexingWarning = new SearchCollectionIndexingWarning(this);
    g->addWidget(d->searchCollectionIndexingWarning, 0);

    d->tabLockedWarning = new TabLockedWarning(this);
    g->addWidget(d->tabLockedWarning, 0);
    connect(d->tabLockedWarning, &TabLockedWarning::unlockTabRequested, this, [this]() {
        setLockTab(false);
        Q_EMIT unlockTabRequested();
        // Fix icon!
    });

    d->mView = new View(this);
    d->mView->setFrameStyle(QFrame::NoFrame);
    d->mView->setSortOrder(&d->mSortOrder);
    d->mView->setObjectName(QStringLiteral("messagealistview"));
    g->addWidget(d->mView, 1);

    connect(d->mView->header(), &QHeaderView::sectionClicked, this, &Widget::slotViewHeaderSectionClicked);
    d->mSearchTimer = nullptr;
}

Widget::~Widget()
{
    d->mView->setStorageModel(nullptr);

    Manager::unregisterWidget(this);

    delete d->mSearchTimer;
    delete d->mTheme;
    delete d->mAggregation;
    delete d->mFilter;
    delete d->mStorageModel;

    delete d;
}

void Widget::slotActivateFilter(Filter *f)
{
    setFilter(f);
    d->quickSearchLine->searchEdit()->setText(f->searchString());
    d->quickSearchLine->setSearchOptions(f->currentOptions());
    d->quickSearchLine->setFilterMessageStatus(f->status());
}

void Widget::slotSaveFilter()
{
    if (d->mFilter) {
        QPointer<FilterNameDialog> dlg = new FilterNameDialog(this);
        dlg->setExistingFilterNames(FilterSavedManager::self()->existingFilterNames());
        if (dlg->exec()) {
            FilterSavedManager::self()->saveFilter(d->mFilter, dlg->filterName(), dlg->iconName());
        }
        delete dlg;
    } else {
        KMessageBox::information(this, i18n("Any filter defined."), i18n("Create Filter"));
    }
}

void Widget::changeQuicksearchVisibility(bool show)
{
    QLineEdit *const lineEdit = d->quickSearchLine->searchEdit();
    if (!show) {
        // if we hide it we do not want to apply the filter,
        // otherwise someone is maybe stuck with x new emails
        // and cannot read it because of filter
        lineEdit->clear();

        // we focus the message list if we hide the searchbar
        d->mView->setFocus(Qt::OtherFocusReason);
    } else {
        // on show: we focus the lineedit for fast filtering
        lineEdit->setFocus(Qt::OtherFocusReason);
        if (d->mFilter) {
            resetFilter();
        }
    }
    d->quickSearchLine->changeQuicksearchVisibility(show);
    MessageListSettings::self()->setShowQuickSearch(show);
}

void Widget::populateStatusFilterCombo()
{
    if (d->mStatusFilterComboPopulationInProgress) {
        return;
    }
    d->mStatusFilterComboPopulationInProgress = true;
    QComboBox *tagFilterComboBox = d->quickSearchLine->tagFilterComboBox();
    d->mCurrentStatusFilterIndex = (tagFilterComboBox->currentIndex() != -1) ? tagFilterComboBox->currentIndex() : 0;
    disconnect(tagFilterComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &Widget::statusSelected);

    tagFilterComboBox->clear();

    fillMessageTagCombo();
}

void Widget::addMessageTagItem(const QPixmap &icon, const QString &text, const QVariant &data)
{
    d->quickSearchLine->tagFilterComboBox()->addItem(icon, text, data);
}

void Widget::setCurrentStatusFilterItem()
{
    d->quickSearchLine->updateComboboxVisibility();
    connect(d->quickSearchLine->tagFilterComboBox(), qOverload<int>(&QComboBox::currentIndexChanged), this, &Widget::statusSelected);
    d->quickSearchLine->tagFilterComboBox()->setCurrentIndex(
        d->mCurrentStatusFilterIndex >= d->quickSearchLine->tagFilterComboBox()->count() ? 0 : d->mCurrentStatusFilterIndex);
    d->mStatusFilterComboPopulationInProgress = false;
}

MessageItem *Widget::currentMessageItem() const
{
    return view()->currentMessageItem();
}

MessageList::Core::QuickSearchLine::SearchOptions Widget::currentOptions() const
{
    return d->quickSearchLine->searchOptions();
}

QVector<Akonadi::MessageStatus> Widget::currentFilterStatus() const
{
    if (d->mFilter) {
        return d->mFilter->status();
    }
    return QVector<Akonadi::MessageStatus>();
}

QString Widget::currentFilterSearchString() const
{
    if (d->mFilter) {
        return d->mFilter->searchString();
    }
    return QString();
}

QString Widget::currentFilterTagId() const
{
    if (d->mFilter) {
        return d->mFilter->tagId();
    }

    return QString();
}

void Widget::Private::setDefaultAggregationForStorageModel(const StorageModel *storageModel)
{
    const Aggregation *opt = Manager::instance()->aggregationForStorageModel(storageModel, &mStorageUsesPrivateAggregation);

    Q_ASSERT(opt);

    delete mAggregation;
    mAggregation = new Aggregation(*opt);

    mView->setAggregation(mAggregation);

    mLastAggregationId = opt->id();
}

void Widget::Private::setDefaultThemeForStorageModel(const StorageModel *storageModel)
{
    const Theme *opt = Manager::instance()->themeForStorageModel(storageModel, &mStorageUsesPrivateTheme);

    Q_ASSERT(opt);

    delete mTheme;
    mTheme = new Theme(*opt);

    mView->setTheme(mTheme);

    mLastThemeId = opt->id();
}

void Widget::Private::checkSortOrder(const StorageModel *storageModel)
{
    if (storageModel && mAggregation && !mSortOrder.validForAggregation(mAggregation)) {
        qCDebug(MESSAGELIST_LOG) << "Could not restore sort order for folder" << storageModel->id();
        mSortOrder = SortOrder::defaultForAggregation(mAggregation, mSortOrder);

        // Change the global sort order if the sort order didn't fit the global aggregation.
        // Otherwise, if it is a per-folder aggregation, make the sort order per-folder too.
        if (mStorageUsesPrivateAggregation) {
            mStorageUsesPrivateSortOrder = true;
        }
        if (mStorageModel) {
            Manager::instance()->saveSortOrderForStorageModel(storageModel, mSortOrder, mStorageUsesPrivateSortOrder);
        }
        switchMessageSorting(mSortOrder.messageSorting(), mSortOrder.messageSortDirection(), -1);
    }
}

void Widget::Private::setDefaultSortOrderForStorageModel(const StorageModel *storageModel)
{
    // Load the sort order from config and update column headers
    mSortOrder = Manager::instance()->sortOrderForStorageModel(storageModel, &mStorageUsesPrivateSortOrder);
    switchMessageSorting(mSortOrder.messageSorting(), mSortOrder.messageSortDirection(), -1);
    checkSortOrder(storageModel);
}

void Widget::saveCurrentSelection()
{
    if (d->mStorageModel) {
        // Save the current selection
        MessageItem *lastSelectedMessageItem = d->mView->currentMessageItem(false);
        if (lastSelectedMessageItem) {
            d->mStorageModel->savePreSelectedMessage(lastSelectedMessageItem->uniqueId());
        }
    }
}

void Widget::setStorageModel(StorageModel *storageModel, PreSelectionMode preSelectionMode)
{
    if (storageModel == d->mStorageModel) {
        return; // nuthin to do here
    }

    d->setDefaultAggregationForStorageModel(storageModel);
    d->setDefaultThemeForStorageModel(storageModel);
    d->setDefaultSortOrderForStorageModel(storageModel);

    if (!d->quickSearchLine->searchEdit()->locked()) {
        if (d->mSearchTimer) {
            d->mSearchTimer->stop();
            delete d->mSearchTimer;
            d->mSearchTimer = nullptr;
        }

        d->quickSearchLine->searchEdit()->clear();

        if (d->mFilter) {
            resetFilter();
        }
    }
    StorageModel *oldModel = d->mStorageModel;

    d->mStorageModel = storageModel;
    d->mView->setStorageModel(d->mStorageModel, preSelectionMode);

    delete oldModel;

    d->quickSearchLine->tagFilterComboBox()->setEnabled(d->mStorageModel);
    d->quickSearchLine->searchEdit()->setEnabled(d->mStorageModel);
    d->quickSearchLine->setContainsOutboundMessages(d->mStorageModel->containsOutboundMessages());
}

StorageModel *Widget::storageModel() const
{
    return d->mStorageModel;
}

QLineEdit *Widget::quickSearch() const
{
    return d->quickSearchLine->searchEdit();
}

View *Widget::view() const
{
    return d->mView;
}

void Widget::themeMenuAboutToShow()
{
    if (!d->mStorageModel) {
        return;
    }

    auto menu = qobject_cast<QMenu *>(sender());
    if (!menu) {
        return;
    }
    themeMenuAboutToShow(menu);
}

void Widget::themeMenuAboutToShow(QMenu *menu)
{
    menu->clear();

    menu->addSection(i18n("Theme"));

    auto grp = new QActionGroup(menu);

    QList<Theme *> sortedThemes = Manager::instance()->themes().values();

    QAction *act;

    std::sort(sortedThemes.begin(), sortedThemes.end(), MessageList::Core::Theme::compareName);

    for (const auto theme : std::as_const(sortedThemes)) {
        act = menu->addAction(theme->name());
        act->setCheckable(true);
        grp->addAction(act);
        act->setChecked(d->mLastThemeId == theme->id());
        act->setData(QVariant(theme->id()));
        connect(act, &QAction::triggered, this, &Widget::themeSelected);
    }

    menu->addSeparator();

    act = menu->addAction(i18n("Configure..."));
    connect(act, &QAction::triggered, this, &Widget::configureThemes);
}

void Widget::setPrivateSortOrderForStorage()
{
    if (!d->mStorageModel) {
        return;
    }

    d->mStorageUsesPrivateSortOrder = !d->mStorageUsesPrivateSortOrder;

    Manager::instance()->saveSortOrderForStorageModel(d->mStorageModel, d->mSortOrder, d->mStorageUsesPrivateSortOrder);
}

void Widget::configureThemes()
{
    auto dialog = new Utils::ConfigureThemesDialog(window());
    dialog->selectTheme(d->mLastThemeId);
    dialog->show();
}

void Widget::themeSelected(bool)
{
    if (!d->mStorageModel) {
        return; // nuthin to do
    }

    auto act = qobject_cast<QAction *>(sender());
    if (!act) {
        return;
    }

    QVariant v = act->data();
    const QString id = v.toString();

    if (id.isEmpty()) {
        return;
    }

    const Theme *opt = Manager::instance()->theme(id);

    delete d->mTheme;
    d->mTheme = new Theme(*opt);

    d->mView->setTheme(d->mTheme);

    d->mLastThemeId = opt->id();

    // mStorageUsesPrivateTheme = false;

    Manager::instance()->saveThemeForStorageModel(d->mStorageModel, opt->id(), d->mStorageUsesPrivateTheme);

    d->mView->reload();
}

void Widget::aggregationMenuAboutToShow()
{
    auto menu = qobject_cast<QMenu *>(sender());
    if (!menu) {
        return;
    }
    aggregationMenuAboutToShow(menu);
}

void Widget::aggregationMenuAboutToShow(QMenu *menu)
{
    menu->clear();

    menu->addSection(i18n("Aggregation"));

    auto grp = new QActionGroup(menu);

    QList<Aggregation *> sortedAggregations = Manager::instance()->aggregations().values();

    QAction *act;

    std::sort(sortedAggregations.begin(), sortedAggregations.end(), MessageList::Core::Aggregation::compareName);

    for (const auto agg : std::as_const(sortedAggregations)) {
        act = menu->addAction(agg->name());
        act->setCheckable(true);
        grp->addAction(act);
        act->setChecked(d->mLastAggregationId == agg->id());
        act->setData(QVariant(agg->id()));
        connect(act, &QAction::triggered, this, &Widget::aggregationSelected);
    }

    menu->addSeparator();

    act = menu->addAction(i18n("Configure..."));
    act->setData(QVariant(QString()));
    connect(act, &QAction::triggered, this, &Widget::aggregationSelected);
}

void Widget::aggregationSelected(bool)
{
    auto act = qobject_cast<QAction *>(sender());
    if (!act) {
        return;
    }

    QVariant v = act->data();
    QString id = v.toString();

    if (id.isEmpty()) {
        auto dialog = new Utils::ConfigureAggregationsDialog(window());
        dialog->selectAggregation(d->mLastAggregationId);
        dialog->show();
        return;
    }

    if (!d->mStorageModel) {
        return; // nuthin to do
    }

    const Aggregation *opt = Manager::instance()->aggregation(id);

    delete d->mAggregation;
    d->mAggregation = new Aggregation(*opt);

    d->mView->setAggregation(d->mAggregation);

    d->mLastAggregationId = opt->id();

    // mStorageUsesPrivateAggregation = false;

    Manager::instance()->saveAggregationForStorageModel(d->mStorageModel, opt->id(), d->mStorageUsesPrivateAggregation);

    // The sort order might not be valid anymore for this aggregation
    d->checkSortOrder(d->mStorageModel);

    d->mView->reload();
}

void Widget::sortOrderMenuAboutToShow()
{
    if (!d->mAggregation) {
        return;
    }

    auto menu = qobject_cast<QMenu *>(sender());
    if (!menu) {
        return;
    }
    sortOrderMenuAboutToShow(menu);
}

void Widget::sortOrderMenuAboutToShow(QMenu *menu)
{
    menu->clear();

    menu->addSection(i18n("Message Sort Order"));

    QActionGroup *grp;
    QAction *act;
    QVector<QPair<QString, int>> options;

    grp = new QActionGroup(menu);

    options = SortOrder::enumerateMessageSortingOptions(d->mAggregation->threading());
    for (const auto &opt : std::as_const(options)) {
        act = menu->addAction(opt.first);
        act->setCheckable(true);
        grp->addAction(act);
        act->setChecked(d->mSortOrder.messageSorting() == opt.second);
        act->setData(QVariant(opt.second));
    }

    connect(grp, &QActionGroup::triggered, this, &Widget::messageSortingSelected);

    options = SortOrder::enumerateMessageSortDirectionOptions(d->mSortOrder.messageSorting());

    if (options.size() >= 2) {
        menu->addSection(i18n("Message Sort Direction"));

        grp = new QActionGroup(menu);
        for (const auto &opt : std::as_const(options)) {
            act = menu->addAction(opt.first);
            act->setCheckable(true);
            grp->addAction(act);
            act->setChecked(d->mSortOrder.messageSortDirection() == opt.second);
            act->setData(QVariant(opt.second));
        }

        connect(grp, &QActionGroup::triggered, this, &Widget::messageSortDirectionSelected);
    }

    options = SortOrder::enumerateGroupSortingOptions(d->mAggregation->grouping());

    if (options.size() >= 2) {
        menu->addSection(i18n("Group Sort Order"));

        grp = new QActionGroup(menu);
        for (const auto &opt : std::as_const(options)) {
            act = menu->addAction(opt.first);
            act->setCheckable(true);
            grp->addAction(act);
            act->setChecked(d->mSortOrder.groupSorting() == opt.second);
            act->setData(QVariant(opt.second));
        }

        connect(grp, &QActionGroup::triggered, this, &Widget::groupSortingSelected);
    }

    options = SortOrder::enumerateGroupSortDirectionOptions(d->mAggregation->grouping(), d->mSortOrder.groupSorting());

    if (options.size() >= 2) {
        menu->addSection(i18n("Group Sort Direction"));

        grp = new QActionGroup(menu);
        for (const auto &opt : std::as_const(options)) {
            act = menu->addAction(opt.first);
            act->setCheckable(true);
            grp->addAction(act);
            act->setChecked(d->mSortOrder.groupSortDirection() == opt.second);
            act->setData(QVariant(opt.second));
        }

        connect(grp, &QActionGroup::triggered, this, &Widget::groupSortDirectionSelected);
    }

    menu->addSeparator();
    act = menu->addAction(i18n("Folder Always Uses This Sort Order"));
    act->setCheckable(true);
    act->setChecked(d->mStorageUsesPrivateSortOrder);
    connect(act, &QAction::triggered, this, &Widget::setPrivateSortOrderForStorage);
}

void Widget::Private::switchMessageSorting(SortOrder::MessageSorting messageSorting, SortOrder::SortDirection sortDirection, int logicalHeaderColumnIndex)
{
    mSortOrder.setMessageSorting(messageSorting);
    mSortOrder.setMessageSortDirection(sortDirection);

    // If the logicalHeaderColumnIndex was specified then we already know which
    // column we should set the sort indicator to. If it wasn't specified (it's -1)
    // then we need to find it out in the theme.

    if (logicalHeaderColumnIndex == -1) {
        // try to find the specified message sorting in the theme columns
        const auto columns = mTheme->columns();
        int idx = 0;

        // First try with a well defined message sorting.

        for (const auto column : std::as_const(columns)) {
            if (!mView->header()->isSectionHidden(idx)) {
                if (column->messageSorting() == messageSorting) {
                    // found a visible column with this message sorting
                    logicalHeaderColumnIndex = idx;
                    break;
                }
            }
            ++idx;
        }

        // if still not found, try again with a wider range
        if (logicalHeaderColumnIndex == -1) {
            idx = 0;
            for (const auto column : std::as_const(columns)) {
                if (!mView->header()->isSectionHidden(idx)) {
                    if (((column->messageSorting() == SortOrder::SortMessagesBySenderOrReceiver)
                         || (column->messageSorting() == SortOrder::SortMessagesByReceiver) || (column->messageSorting() == SortOrder::SortMessagesBySender))
                        && ((messageSorting == SortOrder::SortMessagesBySenderOrReceiver) || (messageSorting == SortOrder::SortMessagesByReceiver)
                            || (messageSorting == SortOrder::SortMessagesBySender))) {
                        // found a visible column with this message sorting
                        logicalHeaderColumnIndex = idx;
                        break;
                    }
                }
                ++idx;
            }
        }
    }

    if (logicalHeaderColumnIndex == -1) {
        // not found: either not a column-based sorting or the related column is hidden
        mView->header()->setSortIndicatorShown(false);
        return;
    }

    mView->header()->setSortIndicatorShown(true);

    if (sortDirection == SortOrder::Ascending) {
        mView->header()->setSortIndicator(logicalHeaderColumnIndex, Qt::AscendingOrder);
    } else {
        mView->header()->setSortIndicator(logicalHeaderColumnIndex, Qt::DescendingOrder);
    }
}

void Widget::messageSortingSelected(QAction *action)
{
    if (!d->mAggregation) {
        return;
    }
    if (!action) {
        return;
    }

    if (!d->mStorageModel) {
        return;
    }

    bool ok;
    auto ord = static_cast<SortOrder::MessageSorting>(action->data().toInt(&ok));

    if (!ok) {
        return;
    }

    d->switchMessageSorting(ord, d->mSortOrder.messageSortDirection(), -1);
    Manager::instance()->saveSortOrderForStorageModel(d->mStorageModel, d->mSortOrder, d->mStorageUsesPrivateSortOrder);

    d->mView->reload();
}

void Widget::messageSortDirectionSelected(QAction *action)
{
    if (!d->mAggregation) {
        return;
    }
    if (!action) {
        return;
    }
    if (!d->mStorageModel) {
        return;
    }

    bool ok;
    auto ord = static_cast<SortOrder::SortDirection>(action->data().toInt(&ok));

    if (!ok) {
        return;
    }

    d->switchMessageSorting(d->mSortOrder.messageSorting(), ord, -1);
    Manager::instance()->saveSortOrderForStorageModel(d->mStorageModel, d->mSortOrder, d->mStorageUsesPrivateSortOrder);

    d->mView->reload();
}

void Widget::groupSortingSelected(QAction *action)
{
    if (!d->mAggregation) {
        return;
    }
    if (!action) {
        return;
    }

    if (!d->mStorageModel) {
        return;
    }

    bool ok;
    auto ord = static_cast<SortOrder::GroupSorting>(action->data().toInt(&ok));

    if (!ok) {
        return;
    }

    d->mSortOrder.setGroupSorting(ord);
    Manager::instance()->saveSortOrderForStorageModel(d->mStorageModel, d->mSortOrder, d->mStorageUsesPrivateSortOrder);

    d->mView->reload();
}

void Widget::groupSortDirectionSelected(QAction *action)
{
    if (!d->mAggregation) {
        return;
    }
    if (!action) {
        return;
    }
    if (!d->mStorageModel) {
        return;
    }

    bool ok;
    auto ord = static_cast<SortOrder::SortDirection>(action->data().toInt(&ok));

    if (!ok) {
        return;
    }

    d->mSortOrder.setGroupSortDirection(ord);
    Manager::instance()->saveSortOrderForStorageModel(d->mStorageModel, d->mSortOrder, d->mStorageUsesPrivateSortOrder);

    d->mView->reload();
}

void Widget::setFilter(Filter *filter)
{
    resetFilter();
    d->mFilter = filter;
    // TODO
    d->mView->model()->setFilter(d->mFilter);
}

void Widget::resetFilter()
{
    delete d->mFilter;
    d->mFilter = nullptr;
    d->mView->model()->setFilter(nullptr);
    d->quickSearchLine->resetFilter();
    d->quickSearchWarning->animatedHide();
}

void Widget::slotViewHeaderSectionClicked(int logicalIndex)
{
    if (!d->mTheme) {
        return;
    }

    if (!d->mAggregation) {
        return;
    }

    if (logicalIndex >= d->mTheme->columns().count()) {
        return;
    }

    if (!d->mStorageModel) {
        return;
    }

    auto column = d->mTheme->column(logicalIndex);
    if (!column) {
        return; // should never happen...
    }

    if (column->messageSorting() == SortOrder::NoMessageSorting) {
        return; // this is a null op.
    }

    if (d->mSortOrder.messageSorting() == column->messageSorting()) {
        // switch sort direction
        if (d->mSortOrder.messageSortDirection() == SortOrder::Ascending) {
            d->switchMessageSorting(d->mSortOrder.messageSorting(), SortOrder::Descending, logicalIndex);
        } else {
            d->switchMessageSorting(d->mSortOrder.messageSorting(), SortOrder::Ascending, logicalIndex);
        }
    } else {
        // keep sort direction but switch sort order
        d->switchMessageSorting(column->messageSorting(), d->mSortOrder.messageSortDirection(), logicalIndex);
    }
    Manager::instance()->saveSortOrderForStorageModel(d->mStorageModel, d->mSortOrder, d->mStorageUsesPrivateSortOrder);

    d->mView->reload();
}

void Widget::themesChanged()
{
    d->setDefaultThemeForStorageModel(d->mStorageModel);

    d->mView->reload();
}

void Widget::aggregationsChanged()
{
    d->setDefaultAggregationForStorageModel(d->mStorageModel);
    d->checkSortOrder(d->mStorageModel);

    d->mView->reload();
}

void Widget::fillMessageTagCombo()
{
    // nothing here: must be overridden in derived classes
    setCurrentStatusFilterItem();
}

void Widget::tagIdSelected(const QVariant &data)
{
    const QString tagId = data.toString();

    if (tagId.isEmpty()) {
        if (d->mFilter) {
            if (d->mFilter->isEmpty()) {
                resetFilter();
                return;
            }
        }
    } else {
        if (!d->mFilter) {
            d->mFilter = new Filter();
        }
        d->mFilter->setTagId(tagId);
    }

    d->mView->model()->setFilter(d->mFilter);
}

void Widget::setLockTab(bool lock)
{
    d->mLockTab = lock;
    if (lock) {
        d->tabLockedWarning->animatedShow();
    } else {
        d->tabLockedWarning->animatedHide();
    }
}

bool Widget::isLocked() const
{
    return d->mLockTab;
}

void Widget::statusSelected(int index)
{
    if (index == 0) {
        resetFilter();
        return;
    }
    tagIdSelected(d->quickSearchLine->tagFilterComboBox()->itemData(index));
    d->mView->model()->setFilter(d->mFilter);
}

void Widget::searchEditTextEdited()
{
    // This slot is called whenever the user edits the search QLineEdit.
    // Since the user is likely to type more than one character
    // so we start the real search after a short delay in order to catch
    // multiple textEdited() signals.

    if (!d->mSearchTimer) {
        d->mSearchTimer = new QTimer(this);
        connect(d->mSearchTimer, &QTimer::timeout, this, &Widget::searchTimerFired);
    } else {
        d->mSearchTimer->stop(); // eventually
    }

    d->mSearchTimer->setSingleShot(true);
    d->mSearchTimer->start(1000);
}

void Widget::slotStatusButtonsClicked()
{
    // We also arbitrarily set tagId to an empty string, though we *could* allow filtering
    // by status AND tag...
    if (d->mFilter) {
        d->mFilter->setTagId(QString());
    }

    auto lst = d->quickSearchLine->status();
    if (lst.isEmpty()) {
        if (d->mFilter) {
            d->mFilter->setStatus(lst);
            if (d->mFilter->isEmpty()) {
                qCDebug(MESSAGELIST_LOG) << " RESET FILTER";
                resetFilter();
                return;
            }
        }
    } else {
        // don't have this status bit
        if (!d->mFilter) {
            d->mFilter = new Filter();
        }
        d->mFilter->setStatus(lst);
    }

    d->mView->model()->setFilter(d->mFilter);
}

void Widget::searchTimerFired()
{
    // A search is pending.

    if (d->mSearchTimer) {
        d->mSearchTimer->stop();
    }

    if (!d->mFilter) {
        d->mFilter = new Filter();
    }

    const QString text = d->quickSearchLine->searchEdit()->text();

    if (!text.isEmpty()) {
        d->quickSearchLine->addCompletionItem(text);
    }

    d->mFilter->setCurrentFolder(d->mCurrentFolder);
    d->mFilter->setSearchString(text, d->quickSearchLine->searchOptions());
    d->quickSearchWarning->setSearchText(text);
    if (d->mFilter->isEmpty()) {
        resetFilter();
        return;
    }

    d->mView->model()->setFilter(d->mFilter);
}

void Widget::searchEditClearButtonClicked()
{
    if (!d->mFilter) {
        return;
    }

    resetFilter();

    d->mView->scrollTo(d->mView->currentIndex(), QAbstractItemView::PositionAtCenter);
}

void Widget::viewMessageSelected(MessageItem *)
{
}

void Widget::viewMessageActivated(MessageItem *)
{
}

void Widget::viewSelectionChanged()
{
}

void Widget::viewMessageListContextPopupRequest(const QVector<MessageItem *> &, const QPoint &)
{
}

void Widget::viewGroupHeaderContextPopupRequest(GroupHeaderItem *, const QPoint &)
{
}

void Widget::viewDragEnterEvent(QDragEnterEvent *)
{
}

void Widget::viewDragMoveEvent(QDragMoveEvent *)
{
}

void Widget::viewDropEvent(QDropEvent *)
{
}

void Widget::viewStartDragRequest()
{
}

void Widget::viewMessageStatusChangeRequest(MessageItem *msg, Akonadi::MessageStatus set, Akonadi::MessageStatus clear)
{
    Q_UNUSED(msg)
    Q_UNUSED(set)
    Q_UNUSED(clear)
}

void Widget::focusQuickSearch(const QString &selectedText)
{
    d->quickSearchLine->focusQuickSearch(selectedText);
}

bool Widget::isThreaded() const
{
    return d->mView->isThreaded();
}

bool Widget::selectionEmpty() const
{
    return d->mView->selectionEmpty();
}

Akonadi::Collection Widget::currentFolder() const
{
    return d->mCurrentFolder;
}

void Widget::setCurrentFolder(const Akonadi::Collection &collection)
{
    if (!d->mLockTab) {
        d->mCurrentFolder = collection;
        d->searchCollectionIndexingWarning->setCollection(collection);
    }
}

bool Widget::searchEditHasFocus() const
{
    return d->quickSearchLine->searchEdit()->hasFocus();
}
