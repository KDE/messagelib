/*
    SPDX-FileCopyrightText: 2009 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "pane.h"

#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>
#include <KToggleAction>
#include <KXMLGUIClient>
#include <QAction>
#include <QIcon>
#include <QMenu>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QApplication>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QTabBar>
#include <QToolButton>

#include "core/model.h"
#include "core/widgets/quicksearchline.h"
#include "messagelistsettings.h"
#include "messagelistutil_p.h"
#include "storagemodel.h"
#include "widget.h"
#include <Akonadi/ETMViewStateSaver>
#include <Akonadi/MessageStatus>

namespace MessageList
{
class Pane::PanePrivate
{
public:
    PanePrivate(Pane *owner)
        : q(owner)
    {
    }

    void setCurrentFolder(const QModelIndex &etmIndex);
    void onNewTabClicked();
    void onCloseTabClicked();
    void activateTab();
    void closeTab(QWidget *);
    void onCurrentTabChanged();
    void onTabContextMenuRequest(const QPoint &pos);
    void activateNextTab();
    void activatePreviousTab();
    void moveTabLeft();
    void moveTabRight();
    void moveTabBackward();
    void moveTabForward();
    void changeQuicksearchVisibility(bool);
    void addActivateTabAction(int i);
    void slotTabCloseRequested(int index);
    [[nodiscard]] QItemSelection mapSelectionFromSource(const QItemSelection &selection) const;
    void updateTabControls();

    Pane *const q;

    KXMLGUIClient *mXmlGuiClient = nullptr;
    KActionMenu *mActionMenu = nullptr;

    QAbstractItemModel *mModel = nullptr;
    QItemSelectionModel *mSelectionModel = nullptr;
    Core::PreSelectionMode mPreSelectionMode = Core::PreSelectLastSelected;

    QHash<Widget *, QItemSelectionModel *> mWidgetSelectionHash;
    QList<const QAbstractProxyModel *> mProxyStack;

    QToolButton *mNewTabButton = nullptr;
    QToolButton *mCloseTabButton = nullptr;
    QAction *mCloseTabAction = nullptr;
    QAction *mActivateNextTabAction = nullptr;
    QAction *mActivatePreviousTabAction = nullptr;
    QAction *mMoveTabLeftAction = nullptr;
    QAction *mMoveTabRightAction = nullptr;
    QString mQuickSearchPlaceHolderMessage;
    bool mPreferEmptyTab = false;
    int mMaxTabCreated = 0;
};
} // namespace MessageList

using namespace Akonadi;
using namespace MessageList;

Pane::Pane(bool restoreSession, QAbstractItemModel *model, QItemSelectionModel *selectionModel, QWidget *parent)
    : QTabWidget(parent)
    , d(new PanePrivate(this))
{
    setDocumentMode(true);
    d->mModel = model;
    d->mSelectionModel = selectionModel;

    // Build the proxy stack
    const auto *proxyModel = qobject_cast<const QAbstractProxyModel *>(d->mSelectionModel->model());

    while (proxyModel) {
        if (proxyModel == d->mModel) {
            break;
        }

        d->mProxyStack << proxyModel;
        const auto nextProxyModel = qobject_cast<const QAbstractProxyModel *>(proxyModel->sourceModel());

        if (!nextProxyModel) {
            // It's the final model in the chain, so it is necessarily the sourceModel.
            Q_ASSERT(proxyModel->sourceModel() == d->mModel);
            break;
        }
        proxyModel = nextProxyModel;
    } // Proxy stack done

    d->mNewTabButton = new QToolButton(this);
    d->mNewTabButton->setIcon(QIcon::fromTheme(QStringLiteral("tab-new")));
    d->mNewTabButton->adjustSize();
    d->mNewTabButton->setToolTip(i18nc("@info:tooltip", "Open a new tab"));
#ifndef QT_NO_ACCESSIBILITY
    d->mNewTabButton->setAccessibleName(i18n("New tab"));
#endif
    setCornerWidget(d->mNewTabButton, Qt::TopLeftCorner);
    connect(d->mNewTabButton, &QToolButton::clicked, this, [this]() {
        d->onNewTabClicked();
    });

    d->mCloseTabButton = new QToolButton(this);
    d->mCloseTabButton->setIcon(QIcon::fromTheme(QStringLiteral("tab-close")));
    d->mCloseTabButton->adjustSize();
    d->mCloseTabButton->setToolTip(i18nc("@info:tooltip", "Close the current tab"));
#ifndef QT_NO_ACCESSIBILITY
    d->mCloseTabButton->setAccessibleName(i18n("Close tab"));
#endif
    setCornerWidget(d->mCloseTabButton, Qt::TopRightCorner);
    connect(d->mCloseTabButton, &QToolButton::clicked, this, [this]() {
        d->onCloseTabClicked();
    });

    setTabsClosable(true);
    connect(this, &Pane::tabCloseRequested, this, [this](int index) {
        d->slotTabCloseRequested(index);
    });

    readConfig(restoreSession);
    setMovable(true);

    connect(this, &Pane::currentChanged, this, [this]() {
        d->onCurrentTabChanged();
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &Pane::customContextMenuRequested, this, [this](const QPoint &point) {
        d->onTabContextMenuRequest(point);
    });

    connect(MessageListSettings::self(), &MessageListSettings::configChanged, this, [this]() {
        d->updateTabControls();
    });

    // connect(this, &QTabWidget::tabBarDoubleClicked, this, &Pane::createNewTab);

    tabBar()->installEventFilter(this);
}

Pane::~Pane()
{
    saveCurrentSelection();
    writeConfig(true);
}

void Pane::PanePrivate::addActivateTabAction(int i)
{
    const QString actionname = QString::asprintf("activate_tab_%02d", i);
    auto action = new QAction(i18nc("@action", "Activate Tab %1", i), q);
    mXmlGuiClient->actionCollection()->addAction(actionname, action);
    mXmlGuiClient->actionCollection()->setDefaultShortcut(action, QKeySequence(QStringLiteral("Alt+%1").arg(i)));
    connect(action, &QAction::triggered, q, [this]() {
        activateTab();
    });
}

void Pane::PanePrivate::slotTabCloseRequested(int index)
{
    QWidget *w = q->widget(index);
    if (w) {
        closeTab(w);
    }
}

void Pane::setXmlGuiClient(KXMLGUIClient *xmlGuiClient)
{
    d->mXmlGuiClient = xmlGuiClient;

    auto const showHideQuicksearch = new KToggleAction(i18n("Show Quick Search Bar"), this);
    d->mXmlGuiClient->actionCollection()->setDefaultShortcut(showHideQuicksearch, Qt::CTRL | Qt::Key_H);
    showHideQuicksearch->setChecked(MessageListSettings::showQuickSearch());

    d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("show_quick_search"), showHideQuicksearch);
    connect(showHideQuicksearch, &KToggleAction::triggered, this, [this](bool state) {
        d->changeQuicksearchVisibility(state);
    });

    for (int i = 0; i < count(); ++i) {
        auto w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->setXmlGuiClient(d->mXmlGuiClient);
        }
    }

    // Setup "View->Message List" actions.
    if (xmlGuiClient) {
        if (d->mActionMenu) {
            d->mXmlGuiClient->actionCollection()->removeAction(d->mActionMenu);
        }
        d->mActionMenu = new KActionMenu(QIcon(), i18n("Message List"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("view_message_list"), d->mActionMenu);
        MessageList::Util::fillViewMenu(d->mActionMenu->menu(), this);

        d->mActionMenu->addSeparator();

        auto action = new QAction(i18nc("@action", "Create New Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("create_new_tab"), action);
        d->mXmlGuiClient->actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
        connect(action, &QAction::triggered, this, [this]() {
            d->onNewTabClicked();
        });
        d->mActionMenu->addAction(action);
        d->mActionMenu->addSeparator();

        d->mMaxTabCreated = count();
        for (int i = 1; i < 10 && i <= count(); ++i) {
            d->addActivateTabAction(i);
        }

        QList<QKeySequence> nextShortcut;
        QList<QKeySequence> prevShortcut;

        QString nextIcon;
        QString prevIcon;
        if (QApplication::isRightToLeft()) {
            nextShortcut.append(KStandardShortcut::tabPrev());
            prevShortcut.append(KStandardShortcut::tabNext());
            nextIcon = QStringLiteral("go-previous-view");
            prevIcon = QStringLiteral("go-next-view");
        } else {
            nextShortcut.append(KStandardShortcut::tabNext());
            prevShortcut.append(KStandardShortcut::tabPrev());
            nextIcon = QStringLiteral("go-next-view");
            prevIcon = QStringLiteral("go-previous-view");
        }

        d->mActivateNextTabAction = new QAction(i18nc("@action", "Activate Next Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("activate_next_tab"), d->mActivateNextTabAction);
        d->mActivateNextTabAction->setEnabled(false);
        d->mActivateNextTabAction->setIcon(QIcon::fromTheme(nextIcon));
        d->mXmlGuiClient->actionCollection()->setDefaultShortcuts(d->mActivateNextTabAction, nextShortcut);
        connect(d->mActivateNextTabAction, &QAction::triggered, this, [this]() {
            d->activateNextTab();
        });
        d->mActionMenu->addAction(d->mActivateNextTabAction);

        d->mActivatePreviousTabAction = new QAction(i18nc("@action", "Activate Previous Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("activate_previous_tab"), d->mActivatePreviousTabAction);
        d->mXmlGuiClient->actionCollection()->setDefaultShortcuts(d->mActivatePreviousTabAction, prevShortcut);
        d->mActivatePreviousTabAction->setIcon(QIcon::fromTheme(prevIcon));
        d->mActivatePreviousTabAction->setEnabled(false);
        connect(d->mActivatePreviousTabAction, &QAction::triggered, this, [this]() {
            d->activatePreviousTab();
        });
        d->mActionMenu->addAction(d->mActivatePreviousTabAction);

        d->mActionMenu->addSeparator();

        d->mMoveTabLeftAction = new QAction(i18nc("@action", "Move Tab Left"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("move_tab_left"), d->mMoveTabLeftAction);
        d->mMoveTabLeftAction->setEnabled(false);
        connect(d->mMoveTabLeftAction, &QAction::triggered, this, [this]() {
            d->moveTabLeft();
        });
        d->mActionMenu->addAction(d->mMoveTabLeftAction);

        d->mMoveTabRightAction = new QAction(i18nc("@action", "Move Tab Right"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("move_tab_right"), d->mMoveTabRightAction);
        d->mMoveTabRightAction->setEnabled(false);
        connect(d->mMoveTabRightAction, &QAction::triggered, this, [this]() {
            d->moveTabRight();
        });
        d->mActionMenu->addAction(d->mMoveTabRightAction);

        d->mActionMenu->addSeparator();

        d->mCloseTabAction = new QAction(i18nc("@action", "Close Tab"), this);
        d->mXmlGuiClient->actionCollection()->addAction(QStringLiteral("close_current_tab"), d->mCloseTabAction);
        d->mXmlGuiClient->actionCollection()->setDefaultShortcuts(d->mCloseTabAction,
                                                                  QList<QKeySequence>()
                                                                      << QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W) << QKeySequence(Qt::CTRL | Qt::Key_W));
        connect(d->mCloseTabAction, &QAction::triggered, this, [this]() {
            d->onCloseTabClicked();
        });
        d->mActionMenu->addAction(d->mCloseTabAction);
        d->mCloseTabAction->setEnabled(false);
    }
}

bool Pane::selectNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter,
                                 MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour,
                                 bool centerItem,
                                 bool loop)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectNextMessageItem(messageTypeFilter, existingSelectionBehaviour, centerItem, loop);
    } else {
        return false;
    }
}

bool Pane::selectPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter,
                                     MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour,
                                     bool centerItem,
                                     bool loop)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectPreviousMessageItem(messageTypeFilter, existingSelectionBehaviour, centerItem, loop);
    } else {
        return false;
    }
}

bool Pane::focusNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->focusNextMessageItem(messageTypeFilter, centerItem, loop);
    } else {
        return false;
    }
}

bool Pane::focusPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->focusPreviousMessageItem(messageTypeFilter, centerItem, loop);
    } else {
        return false;
    }
}

void Pane::selectFocusedMessageItem(bool centerItem)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->selectFocusedMessageItem(centerItem);
    }
}

bool Pane::selectFirstMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectFirstMessageItem(messageTypeFilter, centerItem);
    } else {
        return false;
    }
}

bool Pane::selectLastMessageItem(Core::MessageTypeFilter messageTypeFilter, bool centerItem)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return true;
        }

        return w->selectLastMessageItem(messageTypeFilter, centerItem);
    } else {
        return false;
    }
}

void Pane::selectAll()
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->selectAll();
    }
}

void Pane::setCurrentThreadExpanded(bool expand)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->setCurrentThreadExpanded(expand);
    }
}

void Pane::setAllThreadsExpanded(bool expand)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->setAllThreadsExpanded(expand);
    }
}

void Pane::setAllGroupsExpanded(bool expand)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        if (w->view()->model()->isLoading()) {
            return;
        }

        w->setAllGroupsExpanded(expand);
    }
}

void Pane::focusQuickSearch(const QString &selectedText)
{
    auto w = static_cast<Widget *>(currentWidget());

    if (w) {
        w->focusQuickSearch(selectedText);
    }
}

void Pane::setQuickSearchClickMessage(const QString &msg)
{
    d->mQuickSearchPlaceHolderMessage = msg;
    for (int i = 0; i < count(); ++i) {
        auto w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->setQuickSearchClickMessage(d->mQuickSearchPlaceHolderMessage);
        }
    }
}

void Pane::PanePrivate::setCurrentFolder(const QModelIndex &etmIndex)
{
    if (mPreferEmptyTab) {
        q->createNewTab();
    }

    auto w = static_cast<Widget *>(q->currentWidget());
    QItemSelectionModel *s = mWidgetSelectionHash[w];

    w->saveCurrentSelection();

    // Deselect old before we select new - so that the messagelist can clear first.
    s->clear();
    if (s->selection().isEmpty()) {
        w->view()->model()->setPreSelectionMode(mPreSelectionMode);
    }
    Q_ASSERT(s->model() == etmIndex.model());
    s->select(etmIndex, QItemSelectionModel::Select);

    QString label;
    QIcon icon;
    QString toolTip;
    for (const QModelIndex &index : s->selectedRows()) {
        label += index.data(Qt::DisplayRole).toString() + QLatin1StringView(", ");
    }
    label.chop(2);

    if (label.isEmpty()) {
        label = i18nc("@title:tab Empty messagelist", "Empty");
        icon = QIcon();
    } else if (s->selectedRows().size() == 1) {
        icon = s->selectedRows().first().data(Qt::DecorationRole).value<QIcon>();
        QModelIndex idx = s->selectedRows().first().parent();
        toolTip = label;
        while (idx != QModelIndex()) {
            toolTip = idx.data().toString() + QLatin1Char('/') + toolTip;
            idx = idx.parent();
        }
    } else {
        icon = QIcon::fromTheme(QStringLiteral("folder"));
    }

    const int index = q->indexOf(w);
    q->setTabText(index, label);
    q->setTabIcon(index, icon);
    q->setTabToolTip(index, toolTip);
    if (mPreferEmptyTab) {
        mSelectionModel->select(mapSelectionFromSource(s->selection()), QItemSelectionModel::ClearAndSelect);
    }
    mPreferEmptyTab = false;
}

void Pane::PanePrivate::activateTab()
{
    q->tabBar()->setCurrentIndex(QStringView(q->sender()->objectName()).right(2).toInt() - 1);
}

void Pane::PanePrivate::moveTabRight()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }
    if (QApplication::isRightToLeft()) {
        moveTabForward();
    } else {
        moveTabBackward();
    }
}

void Pane::PanePrivate::moveTabLeft()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }
    if (QApplication::isRightToLeft()) {
        moveTabBackward();
    } else {
        moveTabForward();
    }
}

void Pane::PanePrivate::moveTabForward()
{
    const int currentIndex = q->tabBar()->currentIndex();
    if (currentIndex == q->tabBar()->count() - 1) {
        return;
    }
    q->tabBar()->moveTab(currentIndex, currentIndex + 1);
}

void Pane::PanePrivate::moveTabBackward()
{
    const int currentIndex = q->tabBar()->currentIndex();
    if (currentIndex == 0) {
        return;
    }
    q->tabBar()->moveTab(currentIndex, currentIndex - 1);
}

void Pane::PanePrivate::activateNextTab()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }

    int indexTab = (q->tabBar()->currentIndex() + 1);

    if (indexTab == numberOfTab) {
        indexTab = 0;
    }

    q->tabBar()->setCurrentIndex(indexTab);
}

void Pane::PanePrivate::activatePreviousTab()
{
    const int numberOfTab = q->tabBar()->count();
    if (numberOfTab == 1) {
        return;
    }

    int indexTab = (q->tabBar()->currentIndex() - 1);

    if (indexTab == -1) {
        indexTab = numberOfTab - 1;
    }

    q->tabBar()->setCurrentIndex(indexTab);
}

void Pane::PanePrivate::onNewTabClicked()
{
    q->createNewTab();
}

void Pane::PanePrivate::onCloseTabClicked()
{
    closeTab(q->currentWidget());
}

void Pane::PanePrivate::closeTab(QWidget *w)
{
    if (!w || (q->count() < 2)) {
        return;
    }

    auto wWidget = qobject_cast<Widget *>(w);
    if (wWidget) {
        const bool isLocked = wWidget->isLocked();
        if (isLocked) {
            return;
        }
        wWidget->saveCurrentSelection();
    }

    delete w;
    updateTabControls();
}

void Pane::PanePrivate::changeQuicksearchVisibility(bool show)
{
    for (int i = 0; i < q->count(); ++i) {
        auto w = qobject_cast<Widget *>(q->widget(i));
        if (w) {
            w->changeQuicksearchVisibility(show);
        }
    }
}

bool Pane::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        auto const mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::MiddleButton) {
            return true;
        }
    }
    return QTabWidget::eventFilter(object, event);
}

void Pane::PanePrivate::onCurrentTabChanged()
{
    Q_EMIT q->currentTabChanged();

    auto w = static_cast<Widget *>(q->currentWidget());
    mCloseTabButton->setEnabled(!w->isLocked());

    QItemSelectionModel *s = mWidgetSelectionHash[w];

    mSelectionModel->select(mapSelectionFromSource(s->selection()), QItemSelectionModel::ClearAndSelect);
}

void Pane::PanePrivate::onTabContextMenuRequest(const QPoint &pos)
{
    QTabBar *bar = q->tabBar();
    if (q->count() <= 1) {
        return;
    }

    const int indexBar = bar->tabAt(bar->mapFrom(q, pos));
    if (indexBar == -1) {
        return;
    }

    auto w = qobject_cast<Widget *>(q->widget(indexBar));
    if (!w) {
        return;
    }

    QMenu menu(q);

    QAction *closeTabAction = nullptr;
    if (!w->isLocked()) {
        closeTabAction = menu.addAction(i18nc("@action:inmenu", "Close Tab"));
        closeTabAction->setIcon(QIcon::fromTheme(QStringLiteral("tab-close")));
    }

    QAction *allOtherAction = menu.addAction(i18nc("@action:inmenu", "Close All Other Tabs"));
    allOtherAction->setIcon(QIcon::fromTheme(QStringLiteral("tab-close-other")));

    menu.addSeparator();

    QAction *lockTabAction = menu.addAction(w->isLocked() ? i18nc("@action:inmenu", "Unlock Tab") : i18nc("@action:inmenu", "Lock Tab"));
    lockTabAction->setIcon(w->isLocked() ? QIcon::fromTheme(QStringLiteral("lock")) : QIcon::fromTheme(QStringLiteral("unlock")));

    QAction *action = menu.exec(q->mapToGlobal(pos));

    if (action == allOtherAction) { // Close all other tabs
        QList<Widget *> widgets;
        const int index = q->indexOf(w);

        for (int i = 0; i < q->count(); ++i) {
            if (i == index) {
                continue; // Skip the current one
            }

            auto other = qobject_cast<Widget *>(q->widget(i));
            const bool isLocked = other->isLocked();
            if (!isLocked && other) {
                widgets << other;
            }
        }

        for (Widget *other : std::as_const(widgets)) {
            other->saveCurrentSelection();
            delete other;
        }

        updateTabControls();
    } else if (closeTabAction && (action == closeTabAction)) {
        closeTab(q->widget(indexBar));
    } else if (action == lockTabAction) {
        auto tab = qobject_cast<Widget *>(q->widget(indexBar));
        const bool isLocked = !tab->isLocked();
        tab->setLockTab(isLocked);
        q->setTabIcon(indexBar, isLocked ? QIcon::fromTheme(QStringLiteral("lock")) : QIcon::fromTheme(QStringLiteral("unlock")));
        q->tabBar()->tabButton(indexBar, QTabBar::RightSide)->setEnabled(!isLocked);
        if (q->tabBar()->currentIndex() == indexBar) {
            mCloseTabButton->setEnabled(!isLocked);
        }
    }
}

MessageList::StorageModel *Pane::createStorageModel(QAbstractItemModel *model, QItemSelectionModel *selectionModel, QObject *parent)
{
    return new MessageList::StorageModel(model, selectionModel, parent);
}

Akonadi::Collection Pane::currentFolder() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->currentCollection();
    }
    return {};
}

void Pane::setCurrentFolder(const Akonadi::Collection &collection,
                            const QModelIndex &etmIndex,
                            bool,
                            Core::PreSelectionMode preSelectionMode,
                            const QString &overrideLabel)
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w->isLocked()) {
        d->setCurrentFolder(etmIndex);
        d->mPreSelectionMode = preSelectionMode;
        if (w) {
            w->setCurrentFolder(collection);
            QItemSelectionModel *s = d->mWidgetSelectionHash[w];
            MessageList::StorageModel *m = createStorageModel(d->mModel, s, w);
            w->setStorageModel(m, preSelectionMode);
            if (!overrideLabel.isEmpty()) {
                const int index = indexOf(w);
                setTabText(index, overrideLabel);
            }
        }
    }
}

void Pane::updateTabIconText(const Akonadi::Collection &collection, const QString &label, const QIcon &icon)
{
    for (int i = 0; i < count(); ++i) {
        auto w = qobject_cast<Widget *>(widget(i));
        if (w && (w->currentCollection() == collection)) {
            const int index = indexOf(w);
            setTabText(index, label);
            setTabIcon(index, icon);
        }
    }
}

QItemSelectionModel *Pane::createNewTab()
{
    auto w = new Widget(this);
    w->setXmlGuiClient(d->mXmlGuiClient);

    addTab(w, i18nc("@title:tab Empty messagelist", "Empty"));
    if (!d->mQuickSearchPlaceHolderMessage.isEmpty()) {
        w->setQuickSearchClickMessage(d->mQuickSearchPlaceHolderMessage);
    }
    if (d->mXmlGuiClient && count() < 10) {
        if (d->mMaxTabCreated < count()) {
            d->mMaxTabCreated = count();
            d->addActivateTabAction(d->mMaxTabCreated);
        }
    }

    auto s = new QItemSelectionModel(d->mModel, w);
    MessageList::StorageModel *m = createStorageModel(d->mModel, s, w);
    w->setStorageModel(m);

    d->mWidgetSelectionHash[w] = s;

    connect(w, &Widget::messageSelected, this, &Pane::messageSelected);
    connect(w, &Widget::messageActivated, this, &Pane::messageActivated);
    connect(w, &Widget::selectionChanged, this, &Pane::selectionChanged);
    connect(w, &Widget::messageStatusChangeRequest, this, &Pane::messageStatusChangeRequest);

    connect(w, &Core::Widget::statusMessage, this, &Pane::statusMessage);

    connect(w, &Core::Widget::forceLostFocus, this, &Pane::forceLostFocus);
    connect(w, &Core::Widget::unlockTabRequested, this, [this, w]() {
        for (int i = 0; i < count(); ++i) {
            if (w == qobject_cast<Widget *>(widget(i))) {
                setTabIcon(i, QIcon::fromTheme(QStringLiteral("unlock")));
            }
        }
    });

    d->updateTabControls();
    setCurrentWidget(w);
    return s;
}

QItemSelection Pane::PanePrivate::mapSelectionFromSource(const QItemSelection &selection) const
{
    QItemSelection result = selection;

    using Iterator = QList<const QAbstractProxyModel *>::ConstIterator;

    for (Iterator it = mProxyStack.end() - 1; it != mProxyStack.begin(); --it) {
        result = (*it)->mapSelectionFromSource(result);
    }
    result = mProxyStack.first()->mapSelectionFromSource(result);

    return result;
}

void Pane::PanePrivate::updateTabControls()
{
    const bool enableAction = (q->count() > 1);
    if (enableAction) {
        q->setCornerWidget(mCloseTabButton, Qt::TopRightCorner);
        mCloseTabButton->setVisible(true);
    } else {
        q->setCornerWidget(nullptr, Qt::TopRightCorner);
    }
    if (mCloseTabAction) {
        mCloseTabAction->setEnabled(enableAction);
    }
    if (mActivatePreviousTabAction) {
        mActivatePreviousTabAction->setEnabled(enableAction);
    }
    if (mActivateNextTabAction) {
        mActivateNextTabAction->setEnabled(enableAction);
    }
    if (mMoveTabRightAction) {
        mMoveTabRightAction->setEnabled(enableAction);
    }
    if (mMoveTabLeftAction) {
        mMoveTabLeftAction->setEnabled(enableAction);
    }

    q->tabBar()->setVisible(enableAction);
    if (enableAction) {
        q->setCornerWidget(mNewTabButton, Qt::TopLeftCorner);
        mNewTabButton->setVisible(true);
    } else {
        q->setCornerWidget(nullptr, Qt::TopLeftCorner);
    }

    q->setTabsClosable(true);
    const int numberOfTab(q->count());
    if (numberOfTab == 1) {
        q->tabBar()->tabButton(0, QTabBar::RightSide)->setEnabled(false);
    } else if (numberOfTab > 1) {
        q->tabBar()->tabButton(0, QTabBar::RightSide)->setEnabled(true);
    }
}

Akonadi::Item Pane::currentItem() const
{
    auto w = static_cast<Widget *>(currentWidget());

    if (!w) {
        return {};
    }

    return w->currentItem();
}

KMime::Message::Ptr Pane::currentMessage() const
{
    auto w = static_cast<Widget *>(currentWidget());

    if (!w) {
        return {};
    }

    return w->currentMessage();
}

QList<KMime::Message::Ptr> Pane::selectionAsMessageList(bool includeCollapsedChildren) const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return {};
    }
    return w->selectionAsMessageList(includeCollapsedChildren);
}

Akonadi::Item::List Pane::selectionAsMessageItemList(bool includeCollapsedChildren) const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return {};
    }
    return w->selectionAsMessageItemList(includeCollapsedChildren);
}

QList<Akonadi::Item::Id> Pane::selectionAsListMessageId(bool includeCollapsedChildren) const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return {};
    }
    return w->selectionAsListMessageId(includeCollapsedChildren);
}

QList<qlonglong> Pane::selectionAsMessageItemListId(bool includeCollapsedChildren) const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return {};
    }
    return w->selectionAsMessageItemListId(includeCollapsedChildren);
}

Akonadi::Item::List Pane::currentThreadAsMessageList() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return {};
    }
    return w->currentThreadAsMessageList();
}

Akonadi::Item::List Pane::itemListFromPersistentSet(MessageList::Core::MessageItemSetReference ref)
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->itemListFromPersistentSet(ref);
    }
    return {};
}

void Pane::deletePersistentSet(MessageList::Core::MessageItemSetReference ref)
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        w->deletePersistentSet(ref);
    }
}

void Pane::markMessageItemsAsAboutToBeRemoved(MessageList::Core::MessageItemSetReference ref, bool bMark)
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        w->markMessageItemsAsAboutToBeRemoved(ref, bMark);
    }
}

QList<Akonadi::MessageStatus> Pane::currentFilterStatus() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return {};
    }
    return w->currentFilterStatus();
}

Core::SearchMessageByButtons::SearchOptions Pane::currentOptions() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return Core::SearchMessageByButtons::SearchEveryWhere;
    }
    return w->currentOptions();
}

QString Pane::currentFilterSearchString() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->currentFilterSearchString();
    }
    return {};
}

bool Pane::isThreaded() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->isThreaded();
    }
    return false;
}

bool Pane::selectionEmpty() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->selectionEmpty();
    }
    return false;
}

bool Pane::getSelectionStats(Akonadi::Item::List &selectedItems,
                             Akonadi::Item::List &selectedVisibleItems,
                             bool *allSelectedBelongToSameThread,
                             bool includeCollapsedChildren) const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (!w) {
        return false;
    }

    return w->getSelectionStats(selectedItems, selectedVisibleItems, allSelectedBelongToSameThread, includeCollapsedChildren);
}

MessageList::Core::MessageItemSetReference Pane::selectionAsPersistentSet(bool includeCollapsedChildren) const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->selectionAsPersistentSet(includeCollapsedChildren);
    }
    return -1;
}

MessageList::Core::MessageItemSetReference Pane::currentThreadAsPersistentSet() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->currentThreadAsPersistentSet();
    }
    return -1;
}

void Pane::focusView()
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        QWidget *view = w->view();
        if (view) {
            view->setFocus();
        }
    }
}

void Pane::reloadGlobalConfiguration()
{
    d->updateTabControls();
}

QItemSelectionModel *Pane::currentItemSelectionModel()
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->view()->selectionModel();
    }
    return nullptr;
}

void Pane::resetModelStorage()
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        auto m = static_cast<MessageList::StorageModel *>(w->storageModel());
        if (m) {
            m->resetModelStorage();
        }
    }
}

void Pane::setPreferEmptyTab(bool emptyTab)
{
    d->mPreferEmptyTab = emptyTab;
}

void Pane::saveCurrentSelection()
{
    for (int i = 0; i < count(); ++i) {
        auto w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->saveCurrentSelection();
        }
    }
}

void Pane::updateTagComboBox()
{
    for (int i = 0; i < count(); ++i) {
        auto w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->populateStatusFilterCombo();
        }
    }
}

void Pane::writeConfig(bool restoreSession)
{
    KConfigGroup conf(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListPane"));

    // Delete list before
    const QStringList list = MessageList::MessageListSettings::self()->config()->groupList().filter(QRegularExpression(QStringLiteral("MessageListTab\\d+")));
    for (const QString &group : list) {
        MessageList::MessageListSettings::self()->config()->deleteGroup(group);
    }

    if (restoreSession) {
        conf.writeEntry(QStringLiteral("currentIndex"), currentIndex());

        int elementTab = 0;
        for (int i = 0; i < count(); ++i) {
            auto w = qobject_cast<Widget *>(widget(i));
            if (w && w->currentCollection().isValid()) {
                KConfigGroup grp(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListTab%1").arg(elementTab));
                grp.writeEntry(QStringLiteral("collectionId"), w->currentCollection().id());
                grp.writeEntry(QStringLiteral("HeaderState"), w->view()->header()->saveState());
                elementTab++;
            }
        }
        conf.writeEntry(QStringLiteral("tabNumber"), elementTab);
    }
    conf.sync();
}

void Pane::readConfig(bool restoreSession)
{
    if (MessageList::MessageListSettings::self()->config()->hasGroup(QStringLiteral("MessageListPane"))) {
        KConfigGroup conf(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListPane"));
        const int numberOfTab = conf.readEntry(QStringLiteral("tabNumber"), 0);
        if (numberOfTab == 0) {
            createNewTab();
        } else {
            for (int i = 0; i < numberOfTab; ++i) {
                createNewTab();
                restoreHeaderSettings(i, restoreSession);
            }
            setCurrentIndex(conf.readEntry(QStringLiteral("currentIndex"), 0));
        }
    } else {
        createNewTab();
        restoreHeaderSettings(0, false);
    }
}

void Pane::restoreHeaderSettings(int index, bool restoreSession)
{
    KConfigGroup grp(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListTab%1").arg(index));
    if (grp.exists()) {
        auto w = qobject_cast<Widget *>(widget(index));
        if (w) {
            w->view()->header()->restoreState(grp.readEntry(QStringLiteral("HeaderState"), QByteArray()));
        }
        if (restoreSession) {
            const Akonadi::Collection::Id id = grp.readEntry(QStringLiteral("collectionId"), -1);
            if (id != -1) {
                auto saver = new Akonadi::ETMViewStateSaver;
                saver->setSelectionModel(d->mSelectionModel);
                saver->selectCollections(Akonadi::Collection::List() << Akonadi::Collection(id));
                saver->restoreCurrentItem(QString::fromLatin1("c%1").arg(id));
                saver->restoreState(grp);
            }
        }
    }
}

bool Pane::searchEditHasFocus() const
{
    auto w = static_cast<Widget *>(currentWidget());
    if (w) {
        return w->searchEditHasFocus();
    }
    return false;
}

void Pane::sortOrderMenuAboutToShow()
{
    auto menu = qobject_cast<QMenu *>(sender());
    if (!menu) {
        return;
    }
    const Widget *const w = static_cast<Widget *>(currentWidget());
    w->view()->sortOrderMenuAboutToShow(menu);
}

void Pane::aggregationMenuAboutToShow()
{
    auto menu = qobject_cast<QMenu *>(sender());
    if (!menu) {
        return;
    }
    const Widget *const w = static_cast<Widget *>(currentWidget());
    w->view()->aggregationMenuAboutToShow(menu);
}

void Pane::themeMenuAboutToShow()
{
    auto menu = qobject_cast<QMenu *>(sender());
    if (!menu) {
        return;
    }
    const Widget *const w = static_cast<Widget *>(currentWidget());
    w->view()->themeMenuAboutToShow(menu);
}

void Pane::populateStatusFilterCombo()
{
    for (int i = 0; i < count(); ++i) {
        auto w = qobject_cast<Widget *>(widget(i));
        if (w) {
            w->populateStatusFilterCombo();
        }
    }
}

#include "moc_pane.cpp"
