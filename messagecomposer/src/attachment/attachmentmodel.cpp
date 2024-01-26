/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "attachmentmodel.h"

#include <QMimeData>
#include <QUrl>

#include "messagecomposer_debug.h"
#include <KIO/Global>
#include <KLocalizedString>
#include <QFileDevice>
#include <QTemporaryDir>

#include <KMime/Headers>
#include <KMime/Util>

using namespace MessageComposer;
using namespace MessageCore;

static Qt::CheckState boolToCheckState(bool checked) // local
{
    if (checked) {
        return Qt::Checked;
    } else {
        return Qt::Unchecked;
    }
}

class MessageComposer::AttachmentModel::AttachmentModelPrivate
{
public:
    explicit AttachmentModelPrivate(AttachmentModel *qq);
    ~AttachmentModelPrivate();

    AttachmentPart::List parts;
    QList<QTemporaryDir *> tempDirs;
    AttachmentModel *const q;
    bool modified = false;
    bool encryptEnabled = false;
    bool signEnabled = false;
    bool encryptSelected = false;
    bool signSelected = false;
    bool autoDisplayEnabled = false;
};

AttachmentModel::AttachmentModelPrivate::AttachmentModelPrivate(AttachmentModel *qq)
    : q(qq)
{
}

AttachmentModel::AttachmentModelPrivate::~AttachmentModelPrivate()
{
    // There should be an automatic way to manage the lifetime of these...
    qDeleteAll(tempDirs);
}

AttachmentModel::AttachmentModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d(new AttachmentModelPrivate(this))
{
}

AttachmentModel::~AttachmentModel() = default;

bool AttachmentModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(parent)

    qCDebug(MESSAGECOMPOSER_LOG) << "data has formats" << data->formats() << "urls" << data->urls() << "action" << int(action);

    if (action == Qt::IgnoreAction) {
        return true;
        //} else if( action != Qt::CopyAction ) {
        //  return false;
    }
    // The dropped data is a list of URLs.
    const QList<QUrl> urls = data->urls();
    if (!urls.isEmpty()) {
        Akonadi::Item::List items;
        for (const QUrl &url : urls) {
            const Akonadi::Item item = Akonadi::Item::fromUrl(url);
            if (item.isValid()) {
                items << item;
            }
        }
        if (items.isEmpty()) {
            Q_EMIT attachUrlsRequested(urls);
        } else {
            Q_EMIT attachItemsRequester(items);
        }
        return true;
    } else {
        return false;
    }
}

QMimeData *AttachmentModel::mimeData(const QModelIndexList &indexes) const
{
    qCDebug(MESSAGECOMPOSER_LOG);
    QList<QUrl> urls;
    for (const QModelIndex &index : indexes) {
        if (index.column() != 0) {
            // Avoid processing the same attachment more than once, since the entire
            // row is selected.
            qCWarning(MESSAGECOMPOSER_LOG) << "column != 0. Possibly duplicate rows passed to mimeData().";
            continue;
        }

        const AttachmentPart::Ptr part = d->parts[index.row()];
        QString attachmentName = part->fileName();
        if (attachmentName.isEmpty()) {
            attachmentName = part->name();
        }
        if (attachmentName.isEmpty()) {
            attachmentName = i18n("unnamed attachment");
        }

        auto tempDir = new QTemporaryDir; // Will remove the directory on destruction.
        d->tempDirs.append(tempDir);
        const QString fileName = tempDir->path() + QLatin1Char('/') + attachmentName;
        QFile f(fileName);
        if (!f.open(QIODevice::WriteOnly)) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Cannot write attachment:" << f.errorString();
            continue;
        }
        const QByteArray data = part->data();
        if (f.write(data) != data.length()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Failed to write all data to file!";
            continue;
        }
        f.setPermissions(f.permissions() | QFileDevice::ReadUser | QFileDevice::WriteUser);
        f.close();

        const QUrl url = QUrl::fromLocalFile(fileName);
        qCDebug(MESSAGECOMPOSER_LOG) << " temporary file " << url;
        urls.append(url);
    }

    auto mimeData = new QMimeData;
    mimeData->setUrls(urls);
    return mimeData;
}

QStringList AttachmentModel::mimeTypes() const
{
    const QStringList types = {QStringLiteral("text/uri-list")};
    return types;
}

Qt::DropActions AttachmentModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool AttachmentModel::isModified() const
{
    return d->modified; // TODO actually set modified=true sometime...
}

void AttachmentModel::setModified(bool modified)
{
    d->modified = modified;
}

bool AttachmentModel::isEncryptEnabled() const
{
    return d->encryptEnabled;
}

void AttachmentModel::setEncryptEnabled(bool enabled)
{
    d->encryptEnabled = enabled;
    Q_EMIT encryptEnabled(enabled);
}

bool AttachmentModel::isAutoDisplayEnabled() const
{
    return d->autoDisplayEnabled;
}

void AttachmentModel::setAutoDisplayEnabled(bool enabled)
{
    d->autoDisplayEnabled = enabled;
    Q_EMIT autoDisplayEnabled(enabled);
}

bool AttachmentModel::isSignEnabled() const
{
    return d->signEnabled;
}

void AttachmentModel::setSignEnabled(bool enabled)
{
    d->signEnabled = enabled;
    Q_EMIT signEnabled(enabled);
}

bool AttachmentModel::isEncryptSelected() const
{
    return d->encryptSelected;
}

void AttachmentModel::setEncryptSelected(bool selected)
{
    d->encryptSelected = selected;
    for (AttachmentPart::Ptr part : std::as_const(d->parts)) {
        part->setEncrypted(selected);
    }
    Q_EMIT dataChanged(index(0, EncryptColumn), index(rowCount() - 1, EncryptColumn));
}

bool AttachmentModel::isSignSelected() const
{
    return d->signSelected;
}

void AttachmentModel::setSignSelected(bool selected)
{
    d->signSelected = selected;
    for (AttachmentPart::Ptr part : std::as_const(d->parts)) {
        part->setSigned(selected);
    }
    Q_EMIT dataChanged(index(0, SignColumn), index(rowCount() - 1, SignColumn));
}

QVariant AttachmentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const AttachmentPart::Ptr part = d->parts.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case NameColumn:
            return QVariant::fromValue(part->name().isEmpty() ? part->fileName() : part->name());
        case SizeColumn:
            return QVariant::fromValue(KIO::convertSize(part->size()));
        case EncodingColumn:
            return QVariant::fromValue(KMime::nameForEncoding(part->encoding()));
        case MimeTypeColumn:
            return QVariant::fromValue(part->mimeType());
        default:
            return {};
        }
    } else if (role == Qt::ToolTipRole) {
        return QVariant::fromValue(i18nc("@info:tooltip",
                                         "Name: %1<br>Size: %2<br>Encoding: %3<br>MimeType=%4",
                                         part->name().isEmpty() ? part->fileName() : part->name(),
                                         KIO::convertSize(part->size()),
                                         KMime::nameForEncoding(part->encoding()),
                                         QString::fromLatin1(part->mimeType().data())));
    } else if (role == Qt::CheckStateRole) {
        switch (index.column()) {
        case CompressColumn:
            return QVariant::fromValue(int(boolToCheckState(part->isCompressed())));
        case EncryptColumn:
            return QVariant::fromValue(int(boolToCheckState(part->isEncrypted())));
        case SignColumn:
            return QVariant::fromValue(int(boolToCheckState(part->isSigned())));
        case AutoDisplayColumn:
            return QVariant::fromValue(int(boolToCheckState(part->isInline())));
        default:
            return {};
        }
    } else if (role == AttachmentPartRole) {
        if (index.column() == 0) {
            return QVariant::fromValue(part);
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "AttachmentPartRole and column != 0.";
            return {};
        }
    } else if (role == NameRole) {
        return QVariant::fromValue(part->fileName().isEmpty() ? part->name() : part->fileName());
    } else if (role == SizeRole) {
        return QVariant::fromValue(KIO::convertSize(part->size()));
    } else if (role == EncodingRole) {
        return QVariant::fromValue(KMime::nameForEncoding(part->encoding()));
    } else if (role == MimeTypeRole) {
        return QVariant::fromValue(part->mimeType());
    } else if (role == CompressRole) {
        return QVariant::fromValue(part->isCompressed());
    } else if (role == EncryptRole) {
        return QVariant::fromValue(part->isEncrypted());
    } else if (role == SignRole) {
        return QVariant::fromValue(part->isSigned());
    } else if (role == AutoDisplayRole) {
        return QVariant::fromValue(part->isInline());
    } else {
        return {};
    }
}

bool AttachmentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool emitDataChanged = true;
    AttachmentPart::Ptr part = d->parts[index.row()];

    if (role == Qt::EditRole) {
        switch (index.column()) {
        case NameColumn:
            if (!value.toString().isEmpty()) {
                part->setName(value.toString());
            } else {
                return false;
            }
            break;
        default:
            return false;
        }
    } else if (role == Qt::CheckStateRole) {
        switch (index.column()) {
        case CompressColumn: {
            bool toZip = value.toBool();
            if (toZip != part->isCompressed()) {
                Q_EMIT attachmentCompressRequested(part, toZip);
                emitDataChanged = false; // Will Q_EMIT when the part is updated.
            }
            break;
        }
        case EncryptColumn:
            part->setEncrypted(value.toBool());
            break;
        case SignColumn:
            part->setSigned(value.toBool());
            break;
        case AutoDisplayColumn:
            part->setInline(value.toBool());
            break;
        default:
            break; // Do nothing.
        }
    } else {
        return false;
    }

    if (emitDataChanged) {
        Q_EMIT dataChanged(index, index);
    }
    return true;
}

void AttachmentModel::addAttachment(const AttachmentPart::Ptr &part)
{
    Q_ASSERT(!d->parts.contains(part));
    if (!part->url().isEmpty()) {
        for (const AttachmentPart::Ptr &partElement : std::as_const(d->parts)) {
            if (partElement->url() == part->url()) {
                return;
            }
        }
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    d->parts.append(part);
    endInsertRows();
}

bool AttachmentModel::updateAttachment(const AttachmentPart::Ptr &part)
{
    const int idx = d->parts.indexOf(part);
    if (idx == -1) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Tried to update non-existent part.";
        return false;
    }
    // Emit dataChanged() for the whole row.
    Q_EMIT dataChanged(index(idx, 0), index(idx, LastColumn - 1));
    return true;
}

bool AttachmentModel::replaceAttachment(const AttachmentPart::Ptr &oldPart, const AttachmentPart::Ptr &newPart)
{
    Q_ASSERT(oldPart != newPart);

    const int idx = d->parts.indexOf(oldPart);
    if (idx == -1) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Tried to replace non-existent part.";
        return false;
    }
    d->parts[idx] = newPart;
    // Emit dataChanged() for the whole row.
    Q_EMIT dataChanged(index(idx, 0), index(idx, LastColumn - 1));
    return true;
}

bool AttachmentModel::removeAttachment(const AttachmentPart::Ptr &part)
{
    const int idx = d->parts.indexOf(part);
    if (idx < 0) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Attachment not found.";
        return false;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    d->parts.removeAt(idx);
    endRemoveRows();
    Q_EMIT attachmentRemoved(part);
    return true;
}

AttachmentPart::List AttachmentModel::attachments() const
{
    return d->parts;
}

Qt::ItemFlags AttachmentModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (!index.isValid()) {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }

    if (index.column() == CompressColumn || index.column() == EncryptColumn || index.column() == SignColumn || index.column() == AutoDisplayColumn) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable | defaultFlags;
    } else if (index.column() == NameColumn) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | defaultFlags;
    } else {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
}

QVariant AttachmentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case NameColumn:
        return i18nc("@title column attachment name.", "Name");
    case SizeColumn:
        return i18nc("@title column attachment size.", "Size");
    case EncodingColumn:
        return i18nc("@title column attachment encoding.", "Encoding");
    case MimeTypeColumn:
        return i18nc("@title column attachment type.", "Type");
    case CompressColumn:
        return i18nc("@title column attachment compression checkbox.", "Compress");
    case EncryptColumn:
        return i18nc("@title column attachment encryption checkbox.", "Encrypt");
    case SignColumn:
        return i18nc("@title column attachment signed checkbox.", "Sign");
    case AutoDisplayColumn:
        return i18nc("@title column attachment inlined checkbox.", "Suggest Automatic Display");
    default:
        qCWarning(MESSAGECOMPOSER_LOG) << "Bad column" << section;
        return {};
    }
}

QModelIndex AttachmentModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }
    Q_ASSERT(row >= 0 && row < rowCount());

    if (parent.isValid()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Called with weird parent.";
        return {};
    }

    return createIndex(row, column);
}

QModelIndex AttachmentModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return {}; // No parent.
}

int AttachmentModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0; // Items have no children.
    }
    return d->parts.count();
}

int AttachmentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return LastColumn;
}

#include "moc_attachmentmodel.cpp"
