/*
 *  SPDX-FileCopyrightText: 2026 Laurent Montel <montel@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "messagelist/storagemodel.h"

#include <Akonadi/Collection>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Item>
#include <KMime/Message>
#include <QItemSelectionModel>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTest>

using namespace Qt::Literals::StringLiterals;

namespace
{
// EntityMimeTypeFilterModel asks the column count to the EntityTreeModel.
class EntityModel : public QStandardItemModel
{
public:
    using QStandardItemModel::QStandardItemModel;

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role >= Akonadi::EntityTreeModel::ColumnCountRole
            && role % Akonadi::EntityTreeModel::TerminalUserRole == Akonadi::EntityTreeModel::ColumnCountRole) {
            return 1;
        }
        return QStandardItemModel::data(index, role);
    }
};

QStandardItem *appendCollection(QStandardItemModel *model, Akonadi::Collection::Id id)
{
    Akonadi::Collection collection(id);
    collection.setContentMimeTypes({u"message/rfc822"_s});
    const auto item = new QStandardItem;
    item->setData(QVariant::fromValue(collection), Akonadi::EntityTreeModel::CollectionRole);
    item->setData(id, Akonadi::EntityTreeModel::CollectionIdRole);
    item->setData(Akonadi::Collection::mimeType(), Akonadi::EntityTreeModel::MimeTypeRole);
    model->invisibleRootItem()->appendRow(item);
    return item;
}

void appendItem(QStandardItem *collection, Akonadi::Item::Id id)
{
    Akonadi::Item item(id);
    item.setMimeType(u"message/rfc822"_s);
    // EntityMimeTypeFilterModel drops items without payload.
    item.setPayload(std::make_shared<KMime::Message>());
    const auto row = new QStandardItem;
    row->setData(QVariant::fromValue(item), Akonadi::EntityTreeModel::ItemRole);
    row->setData(id, Akonadi::EntityTreeModel::ItemIdRole);
    row->setData(u"message/rfc822"_s, Akonadi::EntityTreeModel::MimeTypeRole);
    collection->appendRow(row);
}

// Hides odd item ids.
class OddItemFilterModel : public QSortFilterProxyModel
{
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override
    {
        const auto idx = sourceModel()->index(row, 0, parent);
        return idx.data(Akonadi::EntityTreeModel::ItemIdRole).toLongLong() % 2 == 0;
    }
};
}

class StorageModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void shouldFilterItems()
    {
        EntityModel source;
        QStandardItem *collection = appendCollection(&source, 42);
        for (Akonadi::Item::Id id = 1; id <= 4; ++id) {
            appendItem(collection, id);
        }
        QItemSelectionModel selectionModel(&source);
        selectionModel.select(source.index(0, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        MessageList::StorageModel unfiltered(&source, &selectionModel);
        QCOMPARE(unfiltered.rowCount(), 4);
        QCOMPARE(unfiltered.parentCollectionForRow(3).id(), 42);

        QPointer<OddItemFilterModel> filter = new OddItemFilterModel;
        auto storageModel = new MessageList::StorageModel(&source, &selectionModel, filter);
        QCOMPARE(filter->parent(), storageModel);
        QCOMPARE(storageModel->rowCount(), 2);
        QCOMPARE(storageModel->itemForRow(0).id(), 2);
        QCOMPARE(storageModel->itemForRow(1).id(), 4);
        // Maps through the extra proxy down to the collection.
        QCOMPARE(storageModel->parentCollectionForRow(1).id(), 42);

        delete storageModel;
        QVERIFY(filter.isNull());
    }
};

QTEST_MAIN(StorageModelTest)

#include "storagemodeltest.moc"
