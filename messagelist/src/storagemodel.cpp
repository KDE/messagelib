/*
    SPDX-FileCopyrightText: 2009 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "storagemodel.h"

#include <MessageCore/MessageCoreSettings>
#include <MessageCore/StringUtil>

#include <Akonadi/AttributeFactory>
#include <Akonadi/CollectionStatistics>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/ItemModifyJob>
#include <Akonadi/MessageFolderAttribute>
#include <Akonadi/SelectionProxyModel>

#include "core/messageitem.h"
#include "messagelist_debug.h"
#include "messagelistsettings.h"
#include "messagelistutil.h"
#include <KLocalizedString>
#include <QUrl>

#include <QAbstractItemModel>
#include <QAtomicInt>
#include <QCryptographicHash>
#include <QFontDatabase>
#include <QHash>
#include <QItemSelectionModel>
#include <QMimeData>

namespace MessageList
{
class Q_DECL_HIDDEN StorageModel::StorageModelPrivate
{
public:
    void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onSelectionChanged();
    void loadSettings();

    StorageModel *const q;

    QAbstractItemModel *mModel = nullptr;
    QAbstractItemModel *mChildrenFilterModel = nullptr;
    QItemSelectionModel *mSelectionModel = nullptr;

    QHash<Akonadi::Collection::Id, QString> mFolderHash;

    StorageModelPrivate(StorageModel *owner)
        : q(owner)
    {
    }
};
} // namespace MessageList

using namespace Akonadi;
using namespace MessageList;

namespace
{
KMime::Message::Ptr messageForItem(const Akonadi::Item &item)
{
    if (!item.hasPayload<KMime::Message::Ptr>()) {
        qCWarning(MESSAGELIST_LOG) << "Not a message" << item.id() << item.remoteId() << item.mimeType();
        return {};
    }
    return item.payload<KMime::Message::Ptr>();
}
}

static QAtomicInt _k_attributeInitialized;

MessageList::StorageModel::StorageModel(QAbstractItemModel *model, QItemSelectionModel *selectionModel, QObject *parent)
    : Core::StorageModel(parent)
    , d(new StorageModelPrivate(this))
{
    d->mSelectionModel = selectionModel;
    if (_k_attributeInitialized.testAndSetAcquire(0, 1)) {
        AttributeFactory::registerAttribute<MessageFolderAttribute>();
    }

    auto childrenFilter = new Akonadi::SelectionProxyModel(d->mSelectionModel, this);
    childrenFilter->setSourceModel(model);
    childrenFilter->setFilterBehavior(KSelectionProxyModel::ChildrenOfExactSelection);
    d->mChildrenFilterModel = childrenFilter;

    auto itemFilter = new EntityMimeTypeFilterModel(this);
    itemFilter->setSourceModel(childrenFilter);
    itemFilter->addMimeTypeExclusionFilter(Collection::mimeType());
    itemFilter->addMimeTypeInclusionFilter(QStringLiteral("message/rfc822"));
    itemFilter->setHeaderGroup(EntityTreeModel::ItemListHeaders);

    d->mModel = itemFilter;

    qCDebug(MESSAGELIST_LOG) << "Using model:" << model->metaObject()->className();

    connect(d->mModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &id1, const QModelIndex &id2) {
        d->onSourceDataChanged(id1, id2);
    });

    connect(d->mModel, &QAbstractItemModel::layoutAboutToBeChanged, this, &StorageModel::layoutAboutToBeChanged);
    connect(d->mModel, &QAbstractItemModel::layoutChanged, this, &StorageModel::layoutChanged);
    connect(d->mModel, &QAbstractItemModel::modelAboutToBeReset, this, &StorageModel::modelAboutToBeReset);
    connect(d->mModel, &QAbstractItemModel::modelReset, this, &StorageModel::modelReset);

    // Here we assume we'll always get QModelIndex() in the parameters
    connect(d->mModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &StorageModel::rowsAboutToBeInserted);
    connect(d->mModel, &QAbstractItemModel::rowsInserted, this, &StorageModel::rowsInserted);
    connect(d->mModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &StorageModel::rowsAboutToBeRemoved);
    connect(d->mModel, &QAbstractItemModel::rowsRemoved, this, &StorageModel::rowsRemoved);

    connect(d->mSelectionModel, &QItemSelectionModel::selectionChanged, this, [this]() {
        d->onSelectionChanged();
    });

    d->loadSettings();
    connect(MessageListSettings::self(), &MessageListSettings::configChanged, this, [this]() {
        d->loadSettings();
    });
}

MessageList::StorageModel::~StorageModel() = default;

Collection::List MessageList::StorageModel::displayedCollections() const
{
    Collection::List collections;
    const QModelIndexList indexes = d->mSelectionModel->selectedRows();

    collections.reserve(indexes.count());
    for (const QModelIndex &index : indexes) {
        auto c = index.data(EntityTreeModel::CollectionRole).value<Collection>();
        if (c.isValid()) {
            collections << c;
        }
    }

    return collections;
}

QString MessageList::StorageModel::id() const
{
    QStringList ids;
    const QModelIndexList indexes = d->mSelectionModel->selectedRows();

    ids.reserve(indexes.count());
    for (const QModelIndex &index : indexes) {
        auto c = index.data(EntityTreeModel::CollectionRole).value<Collection>();
        if (c.isValid()) {
            ids << QString::number(c.id());
        }
    }

    ids.sort();
    return ids.join(QLatin1Char(':'));
}

bool MessageList::StorageModel::isOutBoundFolder(const Akonadi::Collection &c) const
{
    if (c.hasAttribute<MessageFolderAttribute>() && c.attribute<MessageFolderAttribute>()->isOutboundFolder()) {
        return true;
    }
    return false;
}

bool MessageList::StorageModel::containsOutboundMessages() const
{
    const QModelIndexList indexes = d->mSelectionModel->selectedRows();

    for (const QModelIndex &index : indexes) {
        auto c = index.data(EntityTreeModel::CollectionRole).value<Collection>();
        if (c.isValid()) {
            return isOutBoundFolder(c);
        }
    }

    return false;
}

int MessageList::StorageModel::initialUnreadRowCountGuess() const
{
    const QModelIndexList indexes = d->mSelectionModel->selectedRows();

    int unreadCount = 0;

    for (const QModelIndex &index : indexes) {
        auto c = index.data(EntityTreeModel::CollectionRole).value<Collection>();
        if (c.isValid()) {
            unreadCount += c.statistics().unreadCount();
        }
    }

    return unreadCount;
}

bool MessageList::StorageModel::initializeMessageItem(MessageList::Core::MessageItem *mi, int row, bool bUseReceiver) const
{
    const Akonadi::Item item = itemForRow(row);
    const KMime::Message::Ptr mail = messageForItem(item);
    if (!mail) {
        return false;
    }

    const Collection parentCol = parentCollectionForRow(row);

    QString sender;
    if (auto from = mail->from(false)) {
        sender = from->asUnicodeString();
    }
    QString receiver;
    if (auto to = mail->to(false)) {
        receiver = to->asUnicodeString();
    }

    // Static for speed reasons
    static const QString noSubject = i18nc("displayed as subject when the subject of a mail is empty", "No Subject");
    static const QString unknown(i18nc("displayed when a mail has unknown sender, receiver or date", "Unknown"));

    if (sender.isEmpty()) {
        sender = unknown;
    }
    if (receiver.isEmpty()) {
        receiver = unknown;
    }

    mi->initialSetup(mail->date()->dateTime().toSecsSinceEpoch(), item.size(), sender, receiver, bUseReceiver);
    mi->setItemId(item.id());
    mi->setParentCollectionId(parentCol.id());

    QString subject;
    if (auto subjectMail = mail->subject(false)) {
        subject = subjectMail->asUnicodeString();
        if (subject.isEmpty()) {
            subject = QLatin1Char('(') + noSubject + QLatin1Char(')');
        }
    }

    mi->setSubject(subject);

    auto it = d->mFolderHash.find(item.storageCollectionId());
    if (it == d->mFolderHash.end()) {
        QString folder;
        Collection collection = collectionForId(item.storageCollectionId());
        while (collection.parentCollection().isValid()) {
            folder = collection.displayName() + QLatin1Char('/') + folder;
            collection = collection.parentCollection();
        }
        folder.chop(1);
        it = d->mFolderHash.insert(item.storageCollectionId(), folder);
    }
    mi->setFolder(it.value());

    updateMessageItemData(mi, row);

    return true;
}

static QByteArray md5Encode(const QByteArray &str)
{
    auto trimmed = str.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }

    QCryptographicHash c(QCryptographicHash::Md5);
    c.addData(trimmed);
    return c.result();
}

static QByteArray md5Encode(const QString &str)
{
    auto trimmed = str.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }

    QCryptographicHash c(QCryptographicHash::Md5);
    c.addData(QByteArrayView(reinterpret_cast<const char *>(trimmed.unicode()), sizeof(QChar) * trimmed.length()));
    return c.result();
}

void MessageList::StorageModel::fillMessageItemThreadingData(MessageList::Core::MessageItem *mi, int row, ThreadingDataSubset subset) const
{
    const KMime::Message::Ptr mail = messageForRow(row);
    Q_ASSERT(mail); // We ASSUME that initializeMessageItem has been called successfully...

    switch (subset) {
    case PerfectThreadingReferencesAndSubject: {
        const QString subject = mail->subject()->asUnicodeString();
        const QString strippedSubject = MessageCore::StringUtil::stripOffPrefixes(subject);
        mi->setStrippedSubjectMD5(md5Encode(strippedSubject));
        mi->setSubjectIsPrefixed(subject != strippedSubject);
        // fall through
    }
        [[fallthrough]];
    case PerfectThreadingPlusReferences: {
        const auto refs = mail->references()->identifiers();
        if (!refs.isEmpty()) {
            mi->setReferencesIdMD5(md5Encode(refs.last()));
        }
    }
        [[fallthrough]];
    // fall through
    case PerfectThreadingOnly: {
        mi->setMessageIdMD5(md5Encode(mail->messageID()->identifier()));
        const auto inReplyTos = mail->inReplyTo()->identifiers();
        if (auto inReplyTos = mail->inReplyTo()) {
            if (!inReplyTos->identifiers().isEmpty()) {
                mi->setInReplyToIdMD5(md5Encode(inReplyTos->identifiers().constFirst()));
            }
        }
        break;
    }
    default:
        Q_ASSERT(false); // should never happen
        break;
    }
}

void MessageList::StorageModel::updateMessageItemData(MessageList::Core::MessageItem *mi, int row) const
{
    const Akonadi::Item item = itemForRow(row);

    Akonadi::MessageStatus stat;
    stat.setStatusFromFlags(item.flags());

    mi->setAkonadiItem(item);
    mi->setStatus(stat);

    if (stat.isEncrypted()) {
        mi->setEncryptionState(Core::MessageItem::FullyEncrypted);
    } else {
        mi->setEncryptionState(Core::MessageItem::EncryptionStateUnknown);
    }

    if (stat.isSigned()) {
        mi->setSignatureState(Core::MessageItem::FullySigned);
    } else {
        mi->setSignatureState(Core::MessageItem::SignatureStateUnknown);
    }

    mi->invalidateTagCache();
}

void MessageList::StorageModel::setMessageItemStatus(MessageList::Core::MessageItem *mi, int row, Akonadi::MessageStatus status)
{
    Q_UNUSED(mi)
    Akonadi::Item item = itemForRow(row);
    item.setFlags(status.statusFlags());
    auto job = new ItemModifyJob(item, this);
    job->disableRevisionCheck();
    job->setIgnorePayload(true);
}

QVariant MessageList::StorageModel::data(const QModelIndex &index, int role) const
{
    // We don't provide an implementation for data() in No-Akonadi-KMail.
    // This is because StorageModel must be a wrapper anyway (because columns
    // must be re-mapped and functions not available in a QAbstractItemModel
    // are strictly needed. So when porting to Akonadi this class will
    // either wrap or subclass the MessageModel and implement initializeMessageItem()
    // with appropriate calls to data(). And for No-Akonadi-KMail we still have
    // a somewhat efficient implementation.

    Q_UNUSED(index)
    Q_UNUSED(role)

    return {};
}

int MessageList::StorageModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 1;
    }
    return 0; // this model is flat.
}

QModelIndex MessageList::StorageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, (void *)nullptr);
    }
    return {}; // this model is flat.
}

QModelIndex MessageList::StorageModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return {}; // this model is flat.
}

int MessageList::StorageModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->mModel->rowCount();
    }
    return 0; // this model is flat.
}

QMimeData *MessageList::StorageModel::mimeData(const QList<MessageList::Core::MessageItem *> &items) const
{
    auto data = new QMimeData();
    QList<QUrl> urls;
    urls.reserve(items.count());
    for (MessageList::Core::MessageItem *mi : items) {
        Akonadi::Item item = itemForRow(mi->currentModelIndexRow());
        urls << item.url(Akonadi::Item::UrlWithMimeType);
    }

    data->setUrls(urls);

    return data;
}

void MessageList::StorageModel::StorageModelPrivate::onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_EMIT q->dataChanged(q->index(topLeft.row(), 0), q->index(bottomRight.row(), 0));
}

void MessageList::StorageModel::StorageModelPrivate::onSelectionChanged()
{
    mFolderHash.clear();
    Q_EMIT q->headerDataChanged(Qt::Horizontal, 0, q->columnCount() - 1);
}

void MessageList::StorageModel::StorageModelPrivate::loadSettings()
{
    // Custom/System colors
    MessageListSettings *settings = MessageListSettings::self();

    if (MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
        Core::MessageItem::setUnreadMessageColor(MessageList::Util::unreadDefaultMessageColor());
        Core::MessageItem::setImportantMessageColor(MessageList::Util::importantDefaultMessageColor());
        Core::MessageItem::setToDoMessageColor(MessageList::Util::todoDefaultMessageColor());
    } else {
        Core::MessageItem::setUnreadMessageColor(settings->unreadMessageColor());
        Core::MessageItem::setImportantMessageColor(settings->importantMessageColor());
        Core::MessageItem::setToDoMessageColor(settings->todoMessageColor());
    }

    if (MessageCore::MessageCoreSettings::self()->useDefaultFonts()) {
        Core::MessageItem::setGeneralFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
        Core::MessageItem::setUnreadMessageFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
        Core::MessageItem::setImportantMessageFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
        Core::MessageItem::setToDoMessageFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    } else {
        Core::MessageItem::setGeneralFont(settings->messageListFont());
        Core::MessageItem::setUnreadMessageFont(settings->unreadMessageFont());
        Core::MessageItem::setImportantMessageFont(settings->importantMessageFont());
        Core::MessageItem::setToDoMessageFont(settings->todoMessageFont());
    }
}

Akonadi::Item MessageList::StorageModel::itemForRow(int row) const
{
    return d->mModel->data(d->mModel->index(row, 0), EntityTreeModel::ItemRole).value<Akonadi::Item>();
}

KMime::Message::Ptr MessageList::StorageModel::messageForRow(int row) const
{
    return messageForItem(itemForRow(row));
}

Collection MessageList::StorageModel::parentCollectionForRow(int row) const
{
    auto mimeProxy = static_cast<QAbstractProxyModel *>(d->mModel);
    // This is index mapped to Akonadi::SelectionProxyModel
    const QModelIndex childrenFilterIndex = mimeProxy->mapToSource(d->mModel->index(row, 0));
    Q_ASSERT(childrenFilterIndex.isValid());

    auto childrenProxy = static_cast<QAbstractProxyModel *>(d->mChildrenFilterModel);
    // This is index mapped to ETM
    const QModelIndex etmIndex = childrenProxy->mapToSource(childrenFilterIndex);
    Q_ASSERT(etmIndex.isValid());
    // We cannot possibly refer to top-level collection
    Q_ASSERT(etmIndex.parent().isValid());

    const auto col = etmIndex.parent().data(EntityTreeModel::CollectionRole).value<Collection>();
    Q_ASSERT(col.isValid());

    return col;
}

Akonadi::Collection MessageList::StorageModel::collectionForId(Akonadi::Collection::Id colId) const
{
    // Get ETM
    auto childrenProxy = static_cast<QAbstractProxyModel *>(d->mChildrenFilterModel);
    if (childrenProxy) {
        QAbstractItemModel *etm = childrenProxy->sourceModel();
        if (etm) {
            // get index in EntityTreeModel
            const QModelIndex idx = EntityTreeModel::modelIndexForCollection(etm, Collection(colId));
            if (idx.isValid()) {
                // get and return collection
                return idx.data(EntityTreeModel::CollectionRole).value<Collection>();
            }
        }
    }
    return {};
}

void MessageList::StorageModel::resetModelStorage()
{
    beginResetModel();
    endResetModel();
}

#include "moc_storagemodel.cpp"
