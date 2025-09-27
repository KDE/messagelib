/*
    SPDX-FileCopyrightText: 2007, 2008 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mimetreemodel.h"
using namespace Qt::Literals::StringLiterals;

#include "messageviewer_debug.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/Util>

#include <KMime/Content>
#include <KMime/Message>

#include <KIO/Global>
#include <KLocalizedString>

#include <QIcon>
#include <QMimeData>
#include <QMimeDatabase>
#include <QTemporaryDir>
#include <QUrl>

using namespace MessageViewer;

class Q_DECL_HIDDEN MimeTreeModel::MimeTreeModelPrivate
{
public:
    MimeTreeModelPrivate() = default;

    ~MimeTreeModelPrivate()
    {
        qDeleteAll(tempDirs);
    }

    void clearTempDir()
    {
        qDeleteAll(tempDirs);
        tempDirs.clear();
    }

    QString descriptionForContent(const KMime::Content *content)
    {
        auto const message = dynamic_cast<const KMime::Message *>(content);
        if (message && message->subject()) {
            return message->subject()->asUnicodeString();
        }
        const QString name = MimeTreeParser::NodeHelper::fileName(content);
        if (!name.isEmpty()) {
            return name;
        }
        if (auto ct = content->contentDescription()) {
            const QString desc = ct->asUnicodeString();
            if (!desc.isEmpty()) {
                return desc;
            }
        }
        return i18n("body part");
    }

    QString mimeTypeForContent(const KMime::Content *content)
    {
        if (auto ct = content->contentType()) {
            return QString::fromLatin1(ct->mimeType());
        }
        return {};
    }

    QString typeForContent(const KMime::Content *content)
    {
        if (auto ct = content->contentType()) {
            const QString contentMimeType = QString::fromLatin1(ct->mimeType());
            const auto mimeType = m_mimeDb.mimeTypeForName(contentMimeType);
            if (!mimeType.isValid()) {
                return contentMimeType;
            }
            return mimeType.comment();
        } else {
            return {};
        }
    }

    QString sizeOfContent(const KMime::Content *content)
    {
        if (content->body().isEmpty()) {
            return {};
        }
        return KIO::convertSize(content->body().size());
    }

    QIcon iconForContent(const KMime::Content *content)
    {
        if (auto ct = content->contentType()) {
            auto iconName = MimeTreeParser::Util::iconNameForMimetype(QLatin1StringView(ct->mimeType()));

            auto mimeType = m_mimeDb.mimeTypeForName(QString::fromLatin1(ct->mimeType()));
            if (!mimeType.isValid() || mimeType.name() == QLatin1StringView("application/octet-stream")) {
                const QString name = descriptionForContent(content);
                mimeType = MimeTreeParser::Util::mimetype(name);
            }

            if (mimeType.isValid() && mimeType.name().startsWith(QLatin1StringView("multipart/"))) {
                return QIcon::fromTheme(u"folder"_s);
            } else if (!iconName.isEmpty() && iconName != QLatin1StringView("unknown")) {
                return QIcon::fromTheme(iconName);
            } else if (mimeType.isValid() && !mimeType.iconName().isEmpty()) {
                return QIcon::fromTheme(mimeType.iconName());
            }

            return {};
        } else {
            return {};
        }
    }

    QList<QTemporaryDir *> tempDirs;
    const KMime::Content *root = nullptr;
    QMimeDatabase m_mimeDb;
};

MimeTreeModel::MimeTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d(new MimeTreeModelPrivate)
{
}

MimeTreeModel::~MimeTreeModel() = default;

void MimeTreeModel::setRoot(const KMime::Content *root)
{
    if (d->root != root) {
        beginResetModel();
        d->clearTempDir();
        d->root = root;
        endResetModel();
    }
}

const KMime::Content *MimeTreeModel::root()
{
    return d->root;
}

QModelIndex MimeTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row != 0) {
            return {};
        }
        return createIndex(row, column, d->root);
    }

    auto parentContent = static_cast<const KMime::Content *>(parent.internalPointer());
    if (!parentContent || row < 0 || parentContent->contents().size() <= (std::size_t)row) {
        return {};
    }
    const KMime::Content *content = parentContent->contents()[row];
    return createIndex(row, column, content);
}

QModelIndex MimeTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }
    auto currentContent = static_cast<const KMime::Content *>(index.internalPointer());
    if (!currentContent) {
        return {};
    }

    KMime::ContentIndex currentIndex = d->root->indexForContent(currentContent);
    if (!currentIndex.isValid()) {
        return {};
    }
    currentIndex.up();
    const KMime::Content *parentContent = d->root->content(currentIndex);
    int row = 0;
    if (currentIndex.isValid()) {
        row = currentIndex.up() - 1; // 1 based -> 0 based
    }

    return createIndex(row, 0, parentContent);
}

int MimeTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!d->root) {
        return 0;
    }
    if (!parent.isValid()) {
        return 1;
    }
    auto parentContent = static_cast<const KMime::Content *>(parent.internalPointer());
    if (parentContent) {
        return parentContent->contents().size();
    }
    return 0;
}

int MimeTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant MimeTreeModel::data(const QModelIndex &index, int role) const
{
    auto content = static_cast<const KMime::Content *>(index.internalPointer());
    if (!content) {
        return {};
    }
    if (role == Qt::ToolTipRole) {
        // TODO
        // return d->root->indexForContent( content ).toString();
        return {};
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return d->descriptionForContent(content);
        case 1:
            return d->typeForContent(content);
        case 2:
            return d->sizeOfContent(content);
        }
    }
    if (role == Qt::DecorationRole && index.column() == 0) {
        return d->iconForContent(content);
    }
    if (role == ContentIndexRole) {
        return QVariant::fromValue(d->root->indexForContent(content));
    }
    if (role == ContentRole) {
        return QVariant::fromValue(content);
    }
    if (role == MimeTypeRole) {
        return d->mimeTypeForContent(content);
    }
    if (role == MainBodyPartRole) {
        auto topLevelMsg = dynamic_cast<const KMime::Message *>(d->root);
        if (!topLevelMsg) {
            return false;
        }
        return topLevelMsg->mainBodyPart() == content;
    }
    if (role == AlternativeBodyPartRole) {
        auto topLevelMsg = dynamic_cast<const KMime::Message *>(d->root);
        if (!topLevelMsg) {
            return false;
        }
        return topLevelMsg->mainBodyPart(content->contentType()->mimeType()) == content;
    }
    return {};
}

QVariant MimeTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return i18n("Description");
        case 1:
            return i18n("Type");
        case 2:
            return i18n("Size");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QMimeData *MimeTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QList<QUrl> urls;
    for (const QModelIndex &index : indexes) {
        // create dnd for one item not for all three columns.
        if (index.column() != 0) {
            continue;
        }
        auto content = static_cast<const KMime::Content *>(index.internalPointer());
        if (!content) {
            continue;
        }
        const QByteArray data = content->decodedBody();
        if (data.isEmpty()) {
            continue;
        }

        auto tempDir = new QTemporaryDir; // Will remove the directory on destruction.
        d->tempDirs.append(tempDir);
        const QString fileName = tempDir->path() + u'/' + d->descriptionForContent(content);
        QFile f(fileName);
        if (!f.open(QIODevice::WriteOnly)) {
            qCWarning(MESSAGEVIEWER_LOG) << "Cannot write attachment:" << f.errorString();
            continue;
        }
        if (f.write(data) != data.length()) {
            qCWarning(MESSAGEVIEWER_LOG) << "Failed to write all data to file!";
            continue;
        }
        f.close();

        const QUrl url = QUrl::fromLocalFile(fileName);
        qCDebug(MESSAGEVIEWER_LOG) << " temporary file " << url;
        urls.append(url);
    }

    auto mimeData = new QMimeData;
    mimeData->setUrls(urls);
    return mimeData;
}

Qt::ItemFlags MimeTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    return Qt::ItemIsDragEnabled | defaultFlags;
}

QStringList MimeTreeModel::mimeTypes() const
{
    const QStringList types = {u"text/uri-list"_s};
    return types;
}

#include "moc_mimetreemodel.cpp"
