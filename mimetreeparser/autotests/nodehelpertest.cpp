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
    auto subNode1 = new KMime::Content();
    auto subNode2 = new KMime::Content();
    auto subsubNode0 = new KMime::Content();
    auto subsubNode1 = new KMime::Content();
    auto subsubNode2 = new KMime::Content();
    auto node2ExtraSubNode1 = new KMime::Content();
    auto node2ExtraSubNode2 = new KMime::Content();
    auto node2ExtraSubsubNode = new KMime::Content();
    auto node2ExtraSubsubNode2 = new KMime::Content();
    auto extra = new KMime::Content();
    auto extra2 = new KMime::Content();
    auto subExtra = new KMime::Content();
    auto subsubExtra = new KMime::Content();
    auto subsubExtraNode1 = new KMime::Content();
    auto subsubExtraNode2 = new KMime::Content();

    subNode2->contentType()->setMimeType("multipart/mixed");
    subNode2->appendContent(subsubNode0);
    subNode2->appendContent(subsubNode1);
    subNode2->appendContent(subsubNode2);
    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(subNode1);
    node->appendContent(subNode2);
    subsubExtra->contentType()->setMimeType("multipart/mixed");
    subsubExtra->appendContent(subsubExtraNode1);
    subsubExtra->appendContent(subsubExtraNode2);
    helper.attachExtraContent(node, extra);
    helper.attachExtraContent(node, extra2);
    helper.attachExtraContent(subNode2, subExtra);
    helper.attachExtraContent(subsubNode2, subsubExtra);

    // This simulates Opaque S/MIME signed and encrypted message with attachment
    // (attachment is node2SubsubNode2)
    node2Extra->contentType()->setMimeType("multipart/mixed");
    node2Extra->appendContent(node2ExtraSubNode1);
    node2Extra->appendContent(node2ExtraSubNode2);
    node2ExtraSubNode2->contentType()->setMimeType("multipart/mixed");
    node2ExtraSubNode2->appendContent(new KMime::Content);
    node2ExtraSubNode2->appendContent(node2ExtraSubsubNode);
    node2ExtraSubNode2->appendContent(node2ExtraSubsubNode2);
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

    QCOMPARE(helper.persistentIndex(subNode2), u"2"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2"_s), subNode2);

    QCOMPARE(helper.persistentIndex(subsubNode1), u"2.2"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.2"_s), subsubNode1);

    QCOMPARE(helper.persistentIndex(subsubNode2), u"2.3"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.3"_s), subsubNode2);

    QCOMPARE(helper.persistentIndex(extra), u"e0"_s);
    QCOMPARE(helper.contentFromIndex(node, u"e0"_s), extra);

    QCOMPARE(helper.persistentIndex(extra2), u"e1"_s);
    QCOMPARE(helper.contentFromIndex(node, u"e1"_s), extra2);

    QCOMPARE(helper.persistentIndex(subExtra), u"2:e0"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2:e0"_s), subExtra);

    QCOMPARE(helper.persistentIndex(subsubExtra), u"2.3:e0"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.3:e0"_s), subsubExtra);

    QCOMPARE(helper.persistentIndex(subsubExtraNode2), u"2.3:e0:2"_s);
    QCOMPARE(helper.contentFromIndex(node, u"2.3:e0:2"_s), subsubExtraNode2);

    QCOMPARE(helper.persistentIndex(node2ExtraSubsubNode2), u"e0:2.3"_s);
    QCOMPARE(helper.contentFromIndex(node2, u"e0:2.3"_s), node2ExtraSubsubNode2);

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
    auto subNode = new KMime::Content();
    auto subsubNode = new KMime::Content();
    auto subsubNode2 = new KMime::Content();
    auto extra = new KMime::Content();
    auto extra2 = new KMime::Content();
    auto subExtra = new KMime::Content();
    auto subsubExtra = new KMime::Content();
    auto subsubExtraNode = new KMime::Content();

    subNode->contentType()->setMimeType("multipart/mixed");
    subNode->appendContent(new KMime::Content);
    subNode->appendContent(subsubNode);
    subNode->appendContent(subsubNode2);
    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(new KMime::Content);
    node->appendContent(subNode);
    subsubExtra->contentType()->setMimeType("multipart/mixed");
    subsubExtra->appendContent(new KMime::Content);
    subsubExtra->appendContent(subsubExtraNode);
    helper.attachExtraContent(node, extra);
    helper.attachExtraContent(node, extra2);
    helper.attachExtraContent(subNode, subExtra);
    helper.attachExtraContent(subsubNode2, subsubExtra);

    url = QUrl(QString());
    QCOMPARE(helper.fromHREF(msg, url), node);

    url = QUrl(u"attachment:e0?place=body"_s);
    QCOMPARE(helper.fromHREF(msg, url), extra);

    url = QUrl(u"attachment:2.2?place=body"_s);
    QCOMPARE(helper.fromHREF(msg, url), subsubNode);

    url = QUrl(u"attachment:2.3:e0:2?place=body"_s);
    QCOMPARE(helper.fromHREF(msg, url), subsubExtraNode);

    QCOMPARE(helper.asHREF(node, u"body"_s), u"attachment:?place=body"_s);
    QCOMPARE(helper.asHREF(extra, u"body"_s), u"attachment:e0?place=body"_s);
    QCOMPARE(helper.asHREF(subsubNode, u"body"_s), u"attachment:2.2?place=body"_s);
    QCOMPARE(helper.asHREF(subsubExtraNode, u"body"_s), u"attachment:2.3:e0:2?place=body"_s);
}

void NodeHelperTest::testLocalFiles()
{
    NodeHelper helper;
    KMime::Message::Ptr msg(new KMime::Message);

    KMime::Content *node = msg->topLevel();
    auto subNode = new KMime::Content();
    auto subsubNode = new KMime::Content();
    auto subsubNode2 = new KMime::Content();
    auto extra = new KMime::Content();
    auto extra2 = new KMime::Content();
    auto subExtra = new KMime::Content();
    auto subsubExtra = new KMime::Content();
    auto subsubExtraNode = new KMime::Content();

    subNode->contentType()->setMimeType("multipart/mixed");
    subNode->appendContent(new KMime::Content);
    subNode->appendContent(subsubNode);
    subNode->appendContent(subsubNode2);
    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(new KMime::Content);
    node->appendContent(subNode);
    subsubExtra->contentType()->setMimeType("multipart/mixed");
    subsubExtra->appendContent(new KMime::Content);
    subsubExtra->appendContent(subsubExtraNode);
    helper.attachExtraContent(node, extra);
    helper.attachExtraContent(node, extra2);
    helper.attachExtraContent(subNode, subExtra);
    helper.attachExtraContent(subsubNode2, subsubExtra);

    helper.writeNodeToTempFile(node);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(node)), node);
    helper.writeNodeToTempFile(subNode);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subNode)), subNode);
    helper.writeNodeToTempFile(subsubNode);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubNode)), subsubNode);
    helper.writeNodeToTempFile(subsubNode2);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubNode2)), subsubNode2);
    helper.writeNodeToTempFile(extra);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(extra)), extra);
    helper.writeNodeToTempFile(subExtra);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subExtra)), subExtra);
    helper.writeNodeToTempFile(subsubExtra);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubExtra)), subsubExtra);
    helper.writeNodeToTempFile(subsubExtraNode);
    QCOMPARE(helper.fromHREF(msg, helper.tempFileUrlFromNode(subsubExtraNode)), subsubExtraNode);
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
    msg.from(true)->fromUnicodeString(tlSender);
    auto node = msg.topLevel();
    auto subNode = new KMime::Content();
    auto subExtra = new KMime::Content();

    // Encapsulated message
    auto encMsg = new KMime::Message;
    encMsg->from(true)->fromUnicodeString(encSender);
    auto encNode = encMsg->topLevel();
    auto encSubNode = new KMime::Content();
    auto encSubExtra = new KMime::Content();

    node->contentType()->setMimeType("multipart/mixed");
    node->appendContent(new KMime::Content);
    node->appendContent(subNode);
    node->appendContent(encMsg);
    encNode->appendContent(encSubNode);

    helper.attachExtraContent(subNode, subExtra);
    helper.attachExtraContent(encSubNode, encSubExtra);

    QCOMPARE(helper.fromAsString(node), tlSender);
    QCOMPARE(helper.fromAsString(subNode), tlSender);
    QCOMPARE(helper.fromAsString(subExtra), tlSender);
    QEXPECT_FAIL("", "Returning sender of encapsulated message is not yet implemented", Continue);
    QCOMPARE(helper.fromAsString(encNode), encSender);
    QEXPECT_FAIL("", "Returning sender of encapsulated message is not yet implemented", Continue);
    QCOMPARE(helper.fromAsString(encSubNode), encSender);
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
