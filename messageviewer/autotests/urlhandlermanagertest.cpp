/*
  SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "urlhandlermanagertest.h"

#include "util.h"
#include "viewer/urlhandlermanager_p.h"
#include "viewer/viewer.h"
#include "viewer/viewer_p.h"

#include <MimeTreeParser/BodyPart>
#include <MimeTreeParser/Enums>

#include <MessageViewer/BodyPartURLHandler>

#include <QTest>

using namespace MessageViewer;

class TestBodyPartURLHandler : public Interface::BodyPartURLHandler
{
public:
    QString name() const override
    {
        return QStringLiteral("TestBodyPartURLHandler");
    }

    void testPath(const QString &path) const
    {
        QCOMPARE(path, mPath);
    }

    void testViewer(MessageViewer::Viewer *viewerInstance) const
    {
        QCOMPARE(viewerInstance, mViewerInstance);
    }

    void testContent(KMime::Content *content) const
    {
        QCOMPARE(content, mContent);
    }

    bool handleClick(MessageViewer::Viewer *viewerInstance, MimeTreeParser::Interface::BodyPart *part, const QString &path) const override
    {
        testContent(part->content());
        testViewer(viewerInstance);
        testPath(path);
        return mHandleClick;
    }

    bool handleContextMenuRequest(MimeTreeParser::Interface::BodyPart *part, const QString &path, const QPoint &p) const override
    {
        Q_UNUSED(p)
        testContent(part->content());
        testPath(path);
        return mHandleContextMenuRequest;
    }

    QString statusBarMessage(MimeTreeParser::Interface::BodyPart *part, const QString &path) const override
    {
        testContent(part->content());
        testPath(path);
        return mStatusBarMessageRet;
    }

    QString mPath;
    QString mStatusBarMessageRet;
    bool mHandleClick;
    bool mHandleContextMenuRequest;
    MessageViewer::Viewer *mViewerInstance;
    KMime::Content *mContent;
};

BodyPartUrlHandlerManagerTest::BodyPartUrlHandlerManagerTest(QObject *parent)
    : QObject(parent)
{
}

void BodyPartUrlHandlerManagerTest::testHandleClick_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("path");
    QTest::addColumn<KMime::ContentIndex>("index");
    QTest::addColumn<bool>("ret");

    QTest::newRow("completely_empty") << QString() << QString() << KMime::ContentIndex() << false;
    QTest::newRow("empty") << QStringLiteral("x-kmail:") << QString() << KMime::ContentIndex() << false;
    QTest::newRow("pgpkey") << QStringLiteral("x-kmail:/bodypart/1234/2/pgpkey") << QStringLiteral("pgpkey") << KMime::ContentIndex(QStringLiteral("2"))
                            << true;
    QTest::newRow("test") << QStringLiteral("x-kmail:/bodypart/1234/1/test") << QStringLiteral("test") << KMime::ContentIndex(QStringLiteral("1")) << true;
    QTest::newRow("test_with_arguments") << QStringLiteral("x-kmail:/bodypart/1234/1/test?foo=qua") << QStringLiteral("test?foo=qua")
                                         << KMime::ContentIndex(QStringLiteral("1")) << true;
}

void BodyPartUrlHandlerManagerTest::testHandleClick()
{
    QFETCH(QString, url);
    QFETCH(QString, path);
    QFETCH(KMime::ContentIndex, index);
    QFETCH(bool, ret);

    BodyPartURLHandlerManager manager;
    TestBodyPartURLHandler handler;

    manager.registerHandler(&handler, QLatin1String(""));
    Viewer v(nullptr);
    ViewerPrivate vp(&v, nullptr, nullptr);
    const KMime::Message::Ptr msg(Test::readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox")));
    vp.setMessage(msg, MimeTreeParser::Delayed);

    handler.mViewerInstance = &v;
    handler.mPath = path;
    handler.mHandleClick = true;
    handler.mContent = msg->content(index);

    QCOMPARE(manager.handleClick(QUrl(url), &vp), ret);
    manager.unregisterHandler(&handler);
}

QTEST_MAIN(BodyPartUrlHandlerManagerTest)
