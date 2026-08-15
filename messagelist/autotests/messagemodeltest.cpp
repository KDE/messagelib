/*
 *  SPDX-FileCopyrightText: 2026 Claudio Cambra <claudio.cambra@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "messagelist/messagemodel.h"

#include <Akonadi/EntityTreeModel>
#include <QAbstractItemModelTester>
#include <QStandardItemModel>
#include <QTest>

namespace
{
QStandardItem *appendItem(QStandardItem *parent, Akonadi::Item::Id id)
{
    const auto item = new QStandardItem;
    item->setData(id, Akonadi::EntityTreeModel::ItemIdRole);
    parent->appendRow(item);
    return item;
}
}

class MessageModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void emptyModelIsValid()
    {
        MessageList::MessageModel model;
        QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::Fatal, &model);

        QCOMPARE(model.rowCount(), 0);
        QCOMPARE(model.columnCount(), 1);
        QVERIFY(!model.indexForItemId(1).isValid());
    }

    void threadingModeCanBeChanged()
    {
        MessageList::MessageModel model;

        QCOMPARE(model.threading(), MessageList::Core::Aggregation::PerfectReferencesAndSubject);
        model.setThreading(MessageList::Core::Aggregation::NoThreading);
        QCOMPARE(model.threading(), MessageList::Core::Aggregation::NoThreading);
    }

    void findsItemsRecursively()
    {
        QStandardItemModel source;
        const auto rootItem = source.invisibleRootItem();
        const auto firstThread = appendItem(rootItem, 10);
        const auto firstReply = appendItem(firstThread, 11);
        appendItem(firstReply, 12);
        appendItem(rootItem, 20);

        MessageList::MessageModel model;
        model.setSourceModel(&source);
        QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::Fatal, &model);

        QCOMPARE(model.rowCount(), 2);
        QVERIFY(model.indexForItemId(10).isValid());
        QVERIFY(model.indexForItemId(11).isValid());
        QVERIFY(model.indexForItemId(12).isValid());
        QVERIFY(model.indexForItemId(20).isValid());
        QVERIFY(!model.indexForItemId(0).isValid());
        QVERIFY(!model.indexForItemId(-1).isValid());
        QVERIFY(!model.indexForItemId(999).isValid());
    }

    void resolvesThreadRoots()
    {
        QStandardItemModel source;
        const auto rootItem = source.invisibleRootItem();
        const auto firstThread = appendItem(rootItem, 10);
        const auto firstReply = appendItem(firstThread, 11);
        appendItem(firstReply, 12);

        MessageList::MessageModel model;
        model.setSourceModel(&source);

        const auto rootIndex = model.index(0, 0);
        const auto firstReplyIndex = model.index(0, 0, rootIndex);
        const auto secondReplyIndex = model.index(0, 0, firstReplyIndex);

        QCOMPARE(model.threadRoot(rootIndex), rootIndex);
        QCOMPARE(model.threadRoot(firstReplyIndex), rootIndex);
        QCOMPARE(model.threadRoot(secondReplyIndex), rootIndex);
        QVERIFY(!model.threadRoot({}).isValid());
    }

    void collectsThreadItemIds()
    {
        QStandardItemModel source;
        const auto rootItem = source.invisibleRootItem();
        const auto firstThread = appendItem(rootItem, 10);
        const auto firstReply = appendItem(firstThread, 11);
        appendItem(firstReply, 12);
        appendItem(firstThread, 13);
        appendItem(rootItem, 20);

        MessageList::MessageModel model;
        model.setSourceModel(&source);

        const auto rootIndex = model.index(0, 0);
        const auto expectedThreadItemIds = QList<Akonadi::Item::Id>{10, 11, 12, 13};
        const auto expectedSecondThreadItemIds = QList<Akonadi::Item::Id>{20};
        QCOMPARE(model.threadItemIds(rootIndex), expectedThreadItemIds);
        QCOMPARE(model.threadItemIds(model.index(1, 0)), expectedSecondThreadItemIds);
        QVERIFY(model.threadItemIds({}).isEmpty());
    }
};

QTEST_MAIN(MessageModelTest)

#include "messagemodeltest.moc"
