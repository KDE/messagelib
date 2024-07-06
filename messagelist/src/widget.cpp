/*
    SPDX-FileCopyrightText: 2009 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "widget.h"

#include <Akonadi/Collection>
#include <Akonadi/ItemCopyJob>
#include <Akonadi/ItemMoveJob>

#include "core/messageitem.h"
#include "core/view.h"
#include "storagemodel.h"
#include <messagelistsettings.h>

#include <QAction>
#include <QApplication>
#include <QDrag>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrlQuery>

#include "messagelist_debug.h"
#include <KJob>
#include <KLocalizedString>
#include <KXMLGUIClient>
#include <KXMLGUIFactory>
#include <QIcon>
#include <QMenu>
#include <QUrl>

#include "core/groupheaderitem.h"

#include <Akonadi/Monitor>
#include <Akonadi/Tag>
#include <Akonadi/TagAttribute>
#include <Akonadi/TagFetchJob>
#include <Akonadi/TagFetchScope>

namespace MessageList
{
class MessageList::Widget::WidgetPrivate
{
public:
    WidgetPrivate(Widget *owner)
        : q(owner)
    {
    }

    [[nodiscard]] Akonadi::Item::List selectionAsItems() const;
    [[nodiscard]] Akonadi::Item itemForRow(int row) const;
    [[nodiscard]] KMime::Message::Ptr messageForRow(int row) const;

    Widget *const q;

    int mLastSelectedMessage = -1;
    KXMLGUIClient *mXmlGuiClient = nullptr;
    QModelIndex mGroupHeaderItemIndex;
    Akonadi::Monitor *mMonitor = nullptr;
};
} // namespace MessageList

using namespace MessageList;
using namespace Akonadi;

MessageList::Widget::Widget(QWidget *parent)
    : Core::Widget(parent)
    , d(new WidgetPrivate(this))
{
    populateStatusFilterCombo();

    d->mMonitor = new Akonadi::Monitor(this);
    d->mMonitor->setObjectName(QLatin1StringView("MessageListTagMonitor"));
    d->mMonitor->setTypeMonitored(Akonadi::Monitor::Tags);
    connect(d->mMonitor, &Akonadi::Monitor::tagAdded, this, &Widget::populateStatusFilterCombo);
    connect(d->mMonitor, &Akonadi::Monitor::tagRemoved, this, &Widget::populateStatusFilterCombo);
    connect(d->mMonitor, &Akonadi::Monitor::tagChanged, this, &Widget::populateStatusFilterCombo);
}

MessageList::Widget::~Widget() = default;

void MessageList::Widget::setXmlGuiClient(KXMLGUIClient *xmlGuiClient)
{
    d->mXmlGuiClient = xmlGuiClient;
}

bool MessageList::Widget::canAcceptDrag(const QDropEvent *e)
{
    if (e->source() == view()->viewport()) {
        return false;
    }

    Collection::List collections = static_cast<const MessageList::StorageModel *>(storageModel())->displayedCollections();
    if (collections.size() != 1) {
        return false; // no folder here or too many (in case we can't decide where the drop will end)
    }

    const Collection target = collections.first();

    if ((target.rights() & Collection::CanCreateItem) == 0) {
        return false; // no way to drag into
    }

    const QList<QUrl> urls = e->mimeData()->urls();
    for (const QUrl &url : urls) {
        const Collection collection = Collection::fromUrl(url);
        if (collection.isValid()) { // You're not supposed to drop collections here
            return false;
        } else { // Yay, this is an item!
            QUrlQuery query(url);
            const QString type = query.queryItemValue(QStringLiteral("type"));
            if (!target.contentMimeTypes().contains(type)) {
                return false;
            }
        }
    }

    return true;
}

bool MessageList::Widget::selectNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter,
                                                MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour,
                                                bool centerItem,
                                                bool loop)
{
    return view()->selectNextMessageItem(messageTypeFilter, existingSelectionBehaviour, centerItem, loop);
}

bool MessageList::Widget::selectPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter,
                                                    MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour,
                                                    bool centerItem,
                                                    bool loop)
{
    return view()->selectPreviousMessageItem(messageTypeFilter, existingSelectionBehaviour, centerItem, loop);
}

bool MessageList::Widget::focusNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    return view()->focusNextMessageItem(messageTypeFilter, centerItem, loop);
}

bool MessageList::Widget::focusPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    return view()->focusPreviousMessageItem(messageTypeFilter, centerItem, loop);
}

void MessageList::Widget::selectFocusedMessageItem(bool centerItem)
{
    view()->selectFocusedMessageItem(centerItem);
}

bool MessageList::Widget::selectFirstMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem)
{
    return view()->selectFirstMessageItem(messageTypeFilter, centerItem);
}

bool MessageList::Widget::selectLastMessageItem(Core::MessageTypeFilter messageTypeFilter, bool centerItem)
{
    return view()->selectLastMessageItem(messageTypeFilter, centerItem);
}

void MessageList::Widget::selectAll()
{
    view()->setAllGroupsExpanded(true);
    view()->selectAll();
}

void MessageList::Widget::setCurrentThreadExpanded(bool expand)
{
    view()->setCurrentThreadExpanded(expand);
}

void MessageList::Widget::setAllThreadsExpanded(bool expand)
{
    view()->setAllThreadsExpanded(expand);
}

void MessageList::Widget::setAllGroupsExpanded(bool expand)
{
    view()->setAllGroupsExpanded(expand);
}

void MessageList::Widget::focusQuickSearch(const QString &selectedText)
{
    view()->focusQuickSearch(selectedText);
}

void MessageList::Widget::setQuickSearchClickMessage(const QString &msg)
{
    view()->setQuickSearchClickMessage(msg);
}

void MessageList::Widget::fillMessageTagCombo()
{
    auto fetchJob = new Akonadi::TagFetchJob(this);
    fetchJob->fetchScope().fetchAttribute<Akonadi::TagAttribute>();
    connect(fetchJob, &Akonadi::TagFetchJob::result, this, &Widget::slotTagsFetched);
}

void MessageList::Widget::slotTagsFetched(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGELIST_LOG) << "Failed to load tags " << job->errorString();
        return;
    }
    auto fetchJob = static_cast<Akonadi::TagFetchJob *>(job);

    KConfigGroup conf(MessageList::MessageListSettings::self()->config(), QStringLiteral("MessageListView"));
    const QString tagSelected = conf.readEntry(QStringLiteral("TagSelected"));
    if (tagSelected.isEmpty()) {
        setCurrentStatusFilterItem();
        return;
    }
    const QStringList tagSelectedLst = tagSelected.split(QLatin1Char(','));

    addMessageTagItem(QIcon::fromTheme(QStringLiteral("mail-flag")).pixmap(16, 16),
                      i18nc("Item in list of Akonadi tags, to show all e-mails", "All"),
                      QString());

    QStringList tagFound;
    const auto tags{fetchJob->tags()};
    for (const Akonadi::Tag &akonadiTag : tags) {
        if (tagSelectedLst.contains(akonadiTag.url().url())) {
            tagFound.append(akonadiTag.url().url());
            QString iconName = QStringLiteral("mail-tagged");
            const QString label = akonadiTag.name();
            const QString id = akonadiTag.url().url();
            const auto attr = akonadiTag.attribute<Akonadi::TagAttribute>();
            if (attr) {
                iconName = attr->iconName();
            }
            addMessageTagItem(QIcon::fromTheme(iconName).pixmap(16, 16), label, QVariant(id));
        }
    }
    conf.writeEntry(QStringLiteral("TagSelected"), tagFound);
    conf.sync();

    setCurrentStatusFilterItem();
}

void MessageList::Widget::viewMessageSelected(MessageList::Core::MessageItem *msg)
{
    int row = -1;
    if (msg) {
        row = msg->currentModelIndexRow();
    }

    if (!msg || !msg->isValid() || !storageModel()) {
        d->mLastSelectedMessage = -1;
        Q_EMIT messageSelected(Akonadi::Item());
        return;
    }

    Q_ASSERT(row >= 0);

    d->mLastSelectedMessage = row;

    Q_EMIT messageSelected(d->itemForRow(row)); // this MAY be null
}

void MessageList::Widget::viewMessageActivated(MessageList::Core::MessageItem *msg)
{
    Q_ASSERT(msg); // must not be null
    Q_ASSERT(storageModel());

    if (!msg->isValid()) {
        return;
    }

    int row = msg->currentModelIndexRow();
    Q_ASSERT(row >= 0);

    // The assert below may fail when quickly opening and closing a non-selected thread.
    // This will actually activate the item without selecting it...
    // Q_ASSERT( d->mLastSelectedMessage == row );

    if (d->mLastSelectedMessage != row) {
        // Very ugly. We are activating a non selected message.
        // This is very likely a double click on the plus sign near a thread leader.
        // Dealing with mLastSelectedMessage here would be expensive: it would involve releasing the last selected,
        // emitting signals, handling recursion... ugly.
        // We choose a very simple solution: double clicking on the plus sign near a thread leader does
        // NOT activate the message (i.e open it in a toplevel window) if it isn't previously selected.
        return;
    }

    Q_EMIT messageActivated(d->itemForRow(row)); // this MAY be null
}

void MessageList::Widget::viewSelectionChanged()
{
    Q_EMIT selectionChanged();
    if (!currentMessageItem()) {
        Q_EMIT messageSelected(Akonadi::Item());
    }
}

void MessageList::Widget::viewMessageListContextPopupRequest(const QList<MessageList::Core::MessageItem *> &selectedItems, const QPoint &globalPos)
{
    Q_UNUSED(selectedItems)

    if (!d->mXmlGuiClient) {
        return;
    }

    QMenu *popup = static_cast<QMenu *>(d->mXmlGuiClient->factory()->container(QStringLiteral("akonadi_messagelist_contextmenu"), d->mXmlGuiClient));
    if (popup) {
        popup->exec(globalPos);
    }
}

void MessageList::Widget::viewMessageStatusChangeRequest(MessageList::Core::MessageItem *msg, Akonadi::MessageStatus set, Akonadi::MessageStatus clear)
{
    Q_ASSERT(msg); // must not be null
    Q_ASSERT(storageModel());

    if (!msg->isValid()) {
        return;
    }

    int row = msg->currentModelIndexRow();
    Q_ASSERT(row >= 0);

    Akonadi::Item item = d->itemForRow(row);
    Q_ASSERT(item.isValid());

    Q_EMIT messageStatusChangeRequest(item, set, clear);
}

void MessageList::Widget::viewGroupHeaderContextPopupRequest(MessageList::Core::GroupHeaderItem *ghi, const QPoint &globalPos)
{
    Q_UNUSED(ghi)

    QMenu menu(this);

    QAction *act = nullptr;

    QModelIndex index = view()->model()->index(ghi, 0);
    d->mGroupHeaderItemIndex = index;

    if (view()->isExpanded(index)) {
        act = menu.addAction(i18n("Collapse Group"));
        connect(act, &QAction::triggered, this, &Widget::slotCollapseItem);
    } else {
        act = menu.addAction(i18n("Expand Group"));
        connect(act, &QAction::triggered, this, &Widget::slotExpandItem);
    }

    menu.addSeparator();

    act = menu.addAction(i18n("Expand All Groups"));
    connect(act, &QAction::triggered, view(), &Core::View::slotExpandAllGroups);

    act = menu.addAction(i18n("Collapse All Groups"));
    connect(act, &QAction::triggered, view(), &Core::View::slotCollapseAllGroups);

    menu.addSeparator();
    act = menu.addAction(i18n("Expand All Threads"));
    connect(act, &QAction::triggered, view(), &Core::View::slotExpandAllThreads);
    act = menu.addAction(i18n("Collapse All Threads"));
    connect(act, &QAction::triggered, view(), &Core::View::slotCollapseAllThreads);

    menu.exec(globalPos);
}

void MessageList::Widget::viewDragEnterEvent(QDragEnterEvent *e)
{
    if (!canAcceptDrag(e)) {
        e->ignore();
        return;
    }

    e->accept();
}

void MessageList::Widget::viewDragMoveEvent(QDragMoveEvent *e)
{
    if (!canAcceptDrag(e)) {
        e->ignore();
        return;
    }

    e->accept();
}

enum DragMode { DragCopy, DragMove, DragCancel };

void MessageList::Widget::viewDropEvent(QDropEvent *e)
{
    if (!canAcceptDrag(e)) {
        e->ignore();
        return;
    }

    const QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty()) {
        qCWarning(MESSAGELIST_LOG) << "Could not decode drag data!";
        e->ignore();
        return;
    }

    e->accept();

    int action;
    if ((e->possibleActions() & Qt::MoveAction) == 0) { // We can't move anyway
        action = DragCopy;
    } else {
        const auto keybstate = QApplication::keyboardModifiers();
        if (keybstate & Qt::CTRL) {
            action = DragCopy;
        } else if (keybstate & Qt::SHIFT) {
            action = DragMove;
        } else {
            QMenu menu;
            QAction *moveAction =
                menu.addAction(QIcon::fromTheme(QStringLiteral("edit-move"), QIcon::fromTheme(QStringLiteral("go-jump"))), i18n("&Move Here"));
            QAction *copyAction = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("&Copy Here"));
            menu.addSeparator();
            menu.addAction(QIcon::fromTheme(QStringLiteral("dialog-cancel")), i18n("C&ancel"));

            QAction *menuChoice = menu.exec(QCursor::pos());
            if (menuChoice == moveAction) {
                action = DragMove;
            } else if (menuChoice == copyAction) {
                action = DragCopy;
            } else {
                action = DragCancel;
            }
        }
    }
    if (action == DragCancel) {
        return;
    }

    Collection::List collections = static_cast<const MessageList::StorageModel *>(storageModel())->displayedCollections();
    Collection target = collections.at(0);
    Akonadi::Item::List items;
    items.reserve(urls.count());
    for (const QUrl &url : std::as_const(urls)) {
        items << Akonadi::Item::fromUrl(url);
    }

    if (action == DragCopy) {
        new ItemCopyJob(items, target, this);
    } else if (action == DragMove) {
        new ItemMoveJob(items, target, this);
    }
}

void MessageList::Widget::viewStartDragRequest()
{
    Collection::List collections = static_cast<const MessageList::StorageModel *>(storageModel())->displayedCollections();

    if (collections.isEmpty()) {
        return; // no folder here
    }

    const QList<Core::MessageItem *> selection = view()->selectionAsMessageItemList();
    if (selection.isEmpty()) {
        return;
    }

    bool readOnly = false;

    for (const Collection &c : std::as_const(collections)) {
        // We won't be able to remove items from this collection
        if ((c.rights() & Collection::CanDeleteItem) == 0) {
            // So the drag will be read-only
            readOnly = true;
            break;
        }
    }

    QList<QUrl> urls;
    urls.reserve(selection.count());
    for (Core::MessageItem *mi : selection) {
        const Akonadi::Item i = d->itemForRow(mi->currentModelIndexRow());
        QUrl url = i.url(Akonadi::Item::UrlWithMimeType);
        QUrlQuery query(url);
        query.addQueryItem(QStringLiteral("parent"), QString::number(mi->parentCollectionId()));
        url.setQuery(query);
        urls << url;
    }

    auto mimeData = new QMimeData;
    mimeData->setUrls(urls);

    auto drag = new QDrag(view()->viewport());
    drag->setMimeData(mimeData);

    // Set pixmap
    QPixmap pixmap;
    if (selection.size() == 1) {
        pixmap = QIcon::fromTheme(QStringLiteral("mail-message")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize));
    } else {
        pixmap = QIcon::fromTheme(QStringLiteral("document-multiple")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize));
    }

    // Calculate hotspot (as in Konqueror)
    if (!pixmap.isNull()) {
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
        drag->setPixmap(pixmap);
    }

    if (readOnly) {
        drag->exec(Qt::CopyAction);
    } else {
        drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
}

Akonadi::Item::List MessageList::Widget::WidgetPrivate::selectionAsItems() const
{
    Akonadi::Item::List res;
    const QList<Core::MessageItem *> selection = q->view()->selectionAsMessageItemList();
    res.reserve(selection.count());

    for (Core::MessageItem *mi : std::as_const(selection)) {
        Akonadi::Item i = itemForRow(mi->currentModelIndexRow());
        Q_ASSERT(i.isValid());
        res << i;
    }

    return res;
}

Akonadi::Item MessageList::Widget::WidgetPrivate::itemForRow(int row) const
{
    return static_cast<const MessageList::StorageModel *>(q->storageModel())->itemForRow(row);
}

KMime::Message::Ptr MessageList::Widget::WidgetPrivate::messageForRow(int row) const
{
    return static_cast<const MessageList::StorageModel *>(q->storageModel())->messageForRow(row);
}

Akonadi::Item MessageList::Widget::currentItem() const
{
    Core::MessageItem *mi = view()->currentMessageItem();

    if (mi == nullptr) {
        return {};
    }

    return d->itemForRow(mi->currentModelIndexRow());
}

KMime::Message::Ptr MessageList::Widget::currentMessage() const
{
    Core::MessageItem *mi = view()->currentMessageItem();

    if (mi == nullptr) {
        return {};
    }

    return d->messageForRow(mi->currentModelIndexRow());
}

QList<KMime::Message::Ptr> MessageList::Widget::selectionAsMessageList(bool includeCollapsedChildren) const
{
    QList<KMime::Message::Ptr> lstMiPtr;
    const QList<Core::MessageItem *> lstMi = view()->selectionAsMessageItemList(includeCollapsedChildren);
    if (lstMi.isEmpty()) {
        return lstMiPtr;
    }
    lstMiPtr.reserve(lstMi.count());
    for (Core::MessageItem *it : std::as_const(lstMi)) {
        lstMiPtr.append(d->messageForRow(it->currentModelIndexRow()));
    }
    return lstMiPtr;
}

Akonadi::Item::List MessageList::Widget::selectionAsMessageItemList(bool includeCollapsedChildren) const
{
    Akonadi::Item::List lstMiPtr;
    const QList<Core::MessageItem *> lstMi = view()->selectionAsMessageItemList(includeCollapsedChildren);
    if (lstMi.isEmpty()) {
        return lstMiPtr;
    }
    lstMiPtr.reserve(lstMi.count());
    for (Core::MessageItem *it : std::as_const(lstMi)) {
        lstMiPtr.append(d->itemForRow(it->currentModelIndexRow()));
    }
    return lstMiPtr;
}

QList<qlonglong> MessageList::Widget::selectionAsMessageItemListId(bool includeCollapsedChildren) const
{
    QList<qlonglong> lstMiPtr;
    const QList<Core::MessageItem *> lstMi = view()->selectionAsMessageItemList(includeCollapsedChildren);
    if (lstMi.isEmpty()) {
        return lstMiPtr;
    }
    lstMiPtr.reserve(lstMi.count());
    for (Core::MessageItem *it : std::as_const(lstMi)) {
        lstMiPtr.append(d->itemForRow(it->currentModelIndexRow()).id());
    }
    return lstMiPtr;
}

QList<Akonadi::Item::Id> MessageList::Widget::selectionAsListMessageId(bool includeCollapsedChildren) const
{
    QList<qlonglong> lstMiPtr;
    const QList<Core::MessageItem *> lstMi = view()->selectionAsMessageItemList(includeCollapsedChildren);
    if (lstMi.isEmpty()) {
        return lstMiPtr;
    }
    lstMiPtr.reserve(lstMi.count());
    for (Core::MessageItem *it : std::as_const(lstMi)) {
        lstMiPtr.append(d->itemForRow(it->currentModelIndexRow()).id());
    }
    return lstMiPtr;
}

Akonadi::Item::List MessageList::Widget::currentThreadAsMessageList() const
{
    Akonadi::Item::List lstMiPtr;
    const QList<Core::MessageItem *> lstMi = view()->currentThreadAsMessageItemList();
    if (lstMi.isEmpty()) {
        return lstMiPtr;
    }
    lstMiPtr.reserve(lstMi.count());
    for (Core::MessageItem *it : std::as_const(lstMi)) {
        lstMiPtr.append(d->itemForRow(it->currentModelIndexRow()));
    }
    return lstMiPtr;
}

MessageList::Core::SearchMessageByButtons::SearchOptions MessageList::Widget::currentOptions() const
{
    return view()->currentOptions();
}

QList<Akonadi::MessageStatus> MessageList::Widget::currentFilterStatus() const
{
    return view()->currentFilterStatus();
}

QString MessageList::Widget::currentFilterSearchString() const
{
    return view()->currentFilterSearchString();
}

bool MessageList::Widget::isThreaded() const
{
    return view()->isThreaded();
}

bool MessageList::Widget::selectionEmpty() const
{
    return view()->selectionEmpty();
}

bool MessageList::Widget::getSelectionStats(Akonadi::Item::List &selectedItems,
                                            Akonadi::Item::List &selectedVisibleItems,
                                            bool *allSelectedBelongToSameThread,
                                            bool includeCollapsedChildren) const
{
    if (!storageModel()) {
        return false;
    }

    selectedItems.clear();
    selectedVisibleItems.clear();

    const QList<Core::MessageItem *> selected = view()->selectionAsMessageItemList(includeCollapsedChildren);

    Core::MessageItem *topmost = nullptr;

    *allSelectedBelongToSameThread = true;

    for (Core::MessageItem *it : std::as_const(selected)) {
        const Akonadi::Item item = d->itemForRow(it->currentModelIndexRow());
        selectedItems.append(item);
        if (view()->isDisplayedWithParentsExpanded(it)) {
            selectedVisibleItems.append(item);
        }
        if (topmost == nullptr) {
            topmost = (*it).topmostMessage();
        } else {
            if (topmost != (*it).topmostMessage()) {
                *allSelectedBelongToSameThread = false;
            }
        }
    }
    return true;
}

void MessageList::Widget::deletePersistentSet(MessageList::Core::MessageItemSetReference ref)
{
    view()->deletePersistentSet(ref);
}

void MessageList::Widget::markMessageItemsAsAboutToBeRemoved(MessageList::Core::MessageItemSetReference ref, bool bMark)
{
    QList<Core::MessageItem *> lstPersistent = view()->persistentSetCurrentMessageItemList(ref);
    if (!lstPersistent.isEmpty()) {
        view()->markMessageItemsAsAboutToBeRemoved(lstPersistent, bMark);
    }
}

Akonadi::Item::List MessageList::Widget::itemListFromPersistentSet(MessageList::Core::MessageItemSetReference ref)
{
    Akonadi::Item::List lstItem;
    const QList<Core::MessageItem *> refList = view()->persistentSetCurrentMessageItemList(ref);
    if (!refList.isEmpty()) {
        lstItem.reserve(refList.count());
        for (Core::MessageItem *it : std::as_const(refList)) {
            lstItem.append(d->itemForRow(it->currentModelIndexRow()));
        }
    }
    return lstItem;
}

MessageList::Core::MessageItemSetReference MessageList::Widget::selectionAsPersistentSet(bool includeCollapsedChildren) const
{
    QList<Core::MessageItem *> lstMi = view()->selectionAsMessageItemList(includeCollapsedChildren);
    if (lstMi.isEmpty()) {
        return -1;
    }
    return view()->createPersistentSet(lstMi);
}

MessageList::Core::MessageItemSetReference MessageList::Widget::currentThreadAsPersistentSet() const
{
    QList<Core::MessageItem *> lstMi = view()->currentThreadAsMessageItemList();
    if (lstMi.isEmpty()) {
        return -1;
    }
    return view()->createPersistentSet(lstMi);
}

Akonadi::Collection MessageList::Widget::currentCollection() const
{
    Collection::List collections = static_cast<const MessageList::StorageModel *>(storageModel())->displayedCollections();
    if (collections.size() != 1) {
        return {}; // no folder here or too many (in case we can't decide where the drop will end)
    }
    return collections.first();
}

void MessageList::Widget::slotCollapseItem()
{
    view()->setCollapseItem(d->mGroupHeaderItemIndex);
}

void MessageList::Widget::slotExpandItem()
{
    view()->setExpandItem(d->mGroupHeaderItemIndex);
}

#include "moc_widget.cpp"
