/* SPDX-FileCopyrightText: 2015 Sandro Knauß <bugs@sandroknauss.de>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "nodehelpertest.h"
using namespace Qt::Literals::StringLiterals;

#include "nodehelper.h"

#include <QTest>

using namespace MimeTreeParser;

NodeHelperTest::NodeHelperTest()

{
}

void NodeHelperTest::testPersistentIndex()
{
    NodeHelper helper;

    auto node = new KMime::Content();
    auto node2 = new KMime::Content();
    auto node2Extra = new KMime::Content();
    auto subNode1 = std::make_unique<KMime::Content>();
    auto subNode2 = std::make_unique<KMime::Content>();
    const auto subNode2Ptr = subNode2.get();
    auto subsubNode0 = std::make_unique<KMime::Content>();
    auto subsubNode1 = std::make_unique<KMime::Content>();
    const auto subsubNode1Ptr = subsubNode1.get();
    auto subsubNode2 = std::make_unique<KMime::Content>();
    const auto subsubNode2Ptr = subsubNode2.get();
    auto node2ExtraSubNode1 = std::make_unique<KMime::Content>();
    auto node2ExtraSubNode2 = std::make_unique<KMime::Content>();
    auto node2ExtraSubsubNode = std::make_unique<KMime::Content>();
    auto node2ExtraSubsubNode2 = std::make_unique<KMime::Content>();
    const auto node2ExtraSubsubNode2Ptr = node2ExtraSubsubNode2.get();
    auto extra = new KMime::Content();
    auto extra2 = new KMime::Content();
    auto subExtra = new KMime::Content();
    auto subsubExtra = new KMime::Content();
    auto subsubExtraNode1 = std::make_unique<KMime::Content>();
    auto subsubExtraNode2 = std::make_unique<KMime::Content>();
    const auto subsubExtraNode2Ptr = subsubExtraNode2.get();

    subNode2->contentType()->setMimeType("multipart/mixed");
    subNode2->appendContent(std::move(subsubNode0));
    subNode2->appendContent(std::move(subsubNode1));
    subNode2->appendContent(std::move(subsubNode2));
    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(std::move(subNode1));
    node->appendContent(std::move(subNode2));
    subsubExtra->contentType()->setMimeType("multipart/mixed");
    subsubExtra->appendContent(std::move(subsubExtraNode1));
    subsubExtra->appendContent(std::move(subsubExtraNode2));
    helper.attachExtraContent(node, extra);
    helper.attachExtraContent(node, extra2);
    helper.attachExtraContent(subNode2Ptr, subExtra);
    helper.attachExtraContent(subsubNode2Ptr, subsubExtra);

    // This simulates Opaque S/MIME signed and encrypted message with attachment
    // (attachment is node2SubsubNode2)
    node2Extra->contentType()->setMimeType("multipart/mixed");
    node2Extra->appendContent(std::move(node2ExtraSubNode1));
    node2ExtraSubNode2->contentType()->setMimeType("multipart/mixed");
    node2ExtraSubNode2->appendContent(std::make_unique<KMime::Content>());
    node2ExtraSubNode2->appendContent(std::move(node2ExtraSubsubNode));
    node2ExtraSubNode2->appendContent(std::move(node2ExtraSubsubNode2));
    node2Extra->appendContent(std::move(node2ExtraSubNode2));
    helper.attachExtraContent(node2, node2Extra);

    /*  all content has a internal first child, so indexes starts at 2
     * node                 ""
     * -> subNode1          "1"
     * -> subNode2          "2"
     *    -> subsubNode1    "2.1"
     *    -> subsubNode1    "2.2"
     *    -> subsubNode2    "2.3"
     *
     * node                 ""
     * -> extra             "e0"
     * -> extra2            "e1"
     *
     * subNode              "2"
     * -> subExtra          "2:e0"
     *
     * subsubNode2          "2.3"
     * -> subsubExtra       "2.3:e0"
     *    -> subsubExtraNode1   "2.3:e0:1"
     *    -> subsubExtraNode2   "2.3:e0:2"
     *
     * node2                ""
     *
     * node2                ""
     * -> node2Extra        "e0"
     *    -> node2ExtraSubNode        "e0:2"
     *       -> node2ExtraSubsubNode  "e0:2.2"
     *       -> node2ExtraSubsubNode2 "e0:2.3"
     */

    QCOMPARE(helper.persistentIndex(node), QString());
    QCOMPARE(helper.contentFromIndex(node, QString()), node);

    QCOMPARE(helper.persistentIndex(node->contents()[0]), u"1"_s);
    QCOMPARE(helper.contentFromIndex(node, u"1"_s), node->contents()[0]);

    QCOMPARE(helper.persistentIndex(subNode2Ptr), u"2"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2"_s), subNode2Ptr);

    QCOMPARE(helper.persistentIndex(subsubNode1Ptr), u"2.2"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.2"_s), subsubNode1Ptr);

    QCOMPARE(helper.persistentIndex(subsubNode2Ptr), u"2.3"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.3"_s), subsubNode2Ptr);

    QCOMPARE(helper.persistentIndex(extra), u"e0"_s);
    QCOMPARE(helper.contentFromIndex(node, u"e0"_s), extra);

    QCOMPARE(helper.persistentIndex(extra2), u"e1"_s);
    QCOMPARE(helper.contentFromIndex(node, u"e1"_s), extra2);

    QCOMPARE(helper.persistentIndex(subExtra), u"2:e0"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2:e0"_s), subExtra);

    QCOMPARE(helper.persistentIndex(subsubExtra), u"2.3:e0"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.3:e0"_s), subsubExtra);

    QCOMPARE(helper.persistentIndex(subsubExtraNode2Ptr), u"2.3:e0:2"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.3:e0:2"_s), subsubExtraNode2Ptr);

    QCOMPARE(helper.persistentIndex(node2ExtraSubsubNode2Ptr), u"e0:2.3"_s);
    QCOMPARE(helper.contentFromIndex(node2, u"e0:2.3"_s), node2ExtraSubsubNode2Ptr);

    helper.clear();
    delete node2;
    delete node;
}

void NodeHelperTest::testHREF()
{
    NodeHelper helper;
    KMime::Message::Ptr msg(new KMime::Message);
    QUrl url;

    KMime::Content *node = msg->topLevel();
    auto subNode = std::make_unique<KMime::Content>();
    const auto subNodePtr = subNode.get();
    auto subsubNode = std::make_unique<KMime::Content>();
    const auto subsubNodePtr = subsubNode.get();
    auto subsubNode2 = std::make_unique<KMime::Content>();
    const auto subsubNode2Ptr = subsubNode2.get();
    auto extra = new KMime::Content();
    auto extra2 = new KMime::Content();
    auto subExtra = new KMime::Content();
    auto subsubExtra = new KMime::Content();
    auto subsubExtraNode = std::make_unique<KMime::Content>();
    const auto subsubExtraNodePtr = subsubExtraNode.get();

    subNode->contentType()->setMimeType("multipart/mixed");
    subNode->appendContent(std::make_unique<KMime::Content>());
    subNode->appendContent(std::move(subsubNode));
    subNode->appendContent(std::move(subsubNode2));
    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(std::make_unique<KMime::Content>());
    node->appendContent(std::move(subNode));
    subsubExtra->contentType()->setMimeType("multipart/mixed");
    subsubExtra->appendContent(std::make_unique<KMime::Content>());
    subsubExtra->appendContent(std::move(subsubExtraNode));
    helper.attachExtraContent(node, extra);
    helper.attachExtraContent(node, extra2);
    helper.attachExtraContent(subNodePtr, subExtra);
    helper.attachExtraContent(subsubNode2Ptr, subsubExtra);

    url = QUrl(QString());
    QCOMPARE(helper.fromHREF(msg, url), node);

    url = QUrl(u"attachment:e0?place=body"_s);
    QCOMPARE(helper.fromHREF(msg, url), extra);

    url = QUrl(u"attachment:2.2?place=body"_s);
    QCOMPARE(helper.fromHREF(msg, url), subsubNodePtr);

    url = QUrl(u"attachment:2.3:e0:2?place=body"_s);
    QCOMPARE(helper.fromHREF(msg, url), subsubExtraNodePtr);

    QCOMPARE(helper.asHREF(node, u"body"_s), u"attachment:?place=body"_s);
    QCOMPARE(helper.asHREF(extra, u"body"_s), u"attachment:e0?place=body"_s);
    QCOMPARE(helper.asHREF(subsubNodePtr, u"body"_s), u"attachment:2.2?place=body"_s);
    QCOMPARE(helper.asHREF(subsubExtraNodePtr, u"body"_s), u"attachment:2.3:e0:2?place=body"_s);
}

void NodeHelperTest::testLocalFiles()
{
    NodeHelper helper;
    KMime::Message::Ptr msg(new KMime::Message);

    KMime::Content *node = msg->topLevel();
    auto subNode = std::make_unique<KMime::Content>();
    const auto subNodePtr = subNode.get();
    auto subsubNode = std::make_unique<KMime::Content>();
    const auto subsubNodePtr = subsubNode.get();
    auto subsubNode2 = std::make_unique<KMime::Content>();
    const auto subsubNode2Ptr = subsubNode2.get();
    auto extra = new KMime::Content();
    auto extra2 = new KMime::Content();
    auto subExtra = new KMime::Content();
    auto subsubExtra = new KMime::Content();
    auto subsubExtraNode = std::make_unique<KMime::Content>();
    const auto subsubExtraNodePtr = subsubExtraNode.get();

    subNode->contentType()->setMimeType("multipart/mixed");
    subNode->appendContent(std::make_unique<KMime::Content>());
    subNode->appendContent(std::move(subsubNode));
    subNode->appendContent(std::move(subsubNode2));
    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(std::make_unique<KMime::Content>());
    node->appendContent(std::move(subNode));
    subsubExtra->contentType()->setMimeType("multipart/mixed");
    subsubExtra->appendContent(std::make_unique<KMime::Content>());
    subsubExtra->appendContent(std::move(subsubExtraNode));
    helper.attachExtraContent(node, extra);
    helper.attachExtraContent(node, extra2);
    helper.attachExtraContent(subNodePtr, subExtra);
    helper.attachExtraContent(subsubNode2Ptr, subsubExtra);

    helper.writeNodeToTempFile(node);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(node)), node);
    helper.writeNodeToTempFile(subNodePtr);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subNodePtr)), subNodePtr);
    helper.writeNodeToTempFile(subsubNodePtr);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubNodePtr)), subsubNodePtr);
    helper.writeNodeToTempFile(subsubNode2Ptr);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubNode2Ptr)), subsubNode2Ptr);
    helper.writeNodeToTempFile(extra);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(extra)), extra);
    helper.writeNodeToTempFile(subExtra);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subExtra)), subExtra);
    helper.writeNodeToTempFile(subsubExtra);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubExtra)), subsubExtra);
    helper.writeNodeToTempFile(subsubExtraNodePtr);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubExtraNodePtr)), subsubExtraNodePtr);
}

void NodeHelperTest::testCreateTempDir()
{
    QString path;
    {
        NodeHelper helper;
        path = helper.createTempDir(u"foo"_s);

        QVERIFY(path.endsWith(QLatin1StringView(".index.foo")));
        QVERIFY(QDir(path).exists());
        QVERIFY(QFile(path).permissions() & QFileDevice::WriteUser);
        QVERIFY(QFile(path).permissions() & QFileDevice::ExeUser);
        QVERIFY(QFile(path).permissions() & QFileDevice::ReadUser);
    }
    QVERIFY(!QDir(path).exists());
}

void NodeHelperTest::testFromAsString()
{
    const QString tlSender = u"Foo <foo@example.com>"_s;
    const QString encSender = u"Bar <bar@example.com>"_s;

    NodeHelper helper;

    // msg (KMime::Message)
    //  |- subNode
    //  |- encNode (KMime::Message)
    //      |- encSubNode
    //
    // subNode
    //  |- subExtra
    //
    // encSubNode
    //  |- encSubExtra

    KMime::Message msg;
    msg.from(KMime::CreatePolicy::Create)->fromUnicodeString(tlSender);
    auto node = msg.topLevel();
    auto subNode = std::make_unique<KMime::Content>();
    const auto subNodePtr = subNode.get();
    auto subExtra = new KMime::Content();

    // Encapsulated message
    auto encMsg = std::make_unique<KMime::Message>();
    encMsg->from(KMime::CreatePolicy::Create)->fromUnicodeString(encSender);
    auto encNode = encMsg->topLevel();
    auto encSubNode = std::make_unique<KMime::Content>();
    const auto encSubNodePtr = encSubNode.get();
    auto encSubExtra = new KMime::Content();

    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(std::make_unique<KMime::Content>());
    node->appendContent(std::move(subNode));
    node->appendContent(std::move(encMsg));
    encNode->appendContent(std::move(encSubNode));

    helper.attachExtraContent(subNodePtr, subExtra);
    helper.attachExtraContent(encSubNodePtr, encSubExtra);

    QCOMPARE(helper.fromAsString(node), tlSender);
    QCOMPARE(helper.fromAsString(subNodePtr), tlSender);
    QCOMPARE(helper.fromAsString(subExtra), tlSender);
    QEXPECT_FAIL("", "Returning sender of encapsulated message is not yet implemented", Continue);
    QCOMPARE(helper.fromAsString(encNode), encSender);
    QEXPECT_FAIL("", "Returning sender of encapsulated message is not yet implemented", Continue);
    QCOMPARE(helper.fromAsString(encSubNodePtr), encSender);
    QEXPECT_FAIL("", "Returning sender of encapsulated message is not yet implemented", Continue);
    QCOMPARE(helper.fromAsString(encSubExtra), encSender);
}

void NodeHelperTest::shouldTestExtractAttachmentIndex_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("extractedPath");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("test1") << u"/bla/qttestn28554.index.2.3:0:2/unnamed"_s << u"2.3:0:2"_s;
    QTest::newRow("test2") << u"/bla/qttestn28554.dd.2.3:0:2/unnamed"_s << u"2.3:0:2"_s;
    QTest::newRow("test3") << u"/bla/qttestn28554.dd.2.3:0/unnamed"_s << u"2.3:0"_s;
    QTest::newRow("test4") << u"/bla/qttestn28554.dd.2.3:0:2:4/unnamed"_s << u"2.3:0:2:4"_s;
}

void NodeHelperTest::shouldTestExtractAttachmentIndex()
{
    QFETCH(QString, path);
    QFETCH(QString, extractedPath);
    NodeHelper helper;
    QCOMPARE(extractedPath, helper.extractAttachmentIndex(path));
}

QTEST_GUILESS_MAIN(NodeHelperTest)

#include "moc_nodehelpertest.cpp"
