/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "viewertest.h"
#include "messageviewer/viewer.h"
#include <KActionCollection>
#include <QTest>
#include <qtestmouse.h>

ViewerTest::ViewerTest() = default;

void ViewerTest::shouldHaveDefaultValuesOnCreation()
{
    auto viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(this));
    viewer->show();
    QVERIFY(QTest::qWaitForWindowExposed(viewer));

    auto mViewer = viewer->findChild<QWidget *>(QStringLiteral("mViewer"));
    QVERIFY(mViewer);
    QCOMPARE(mViewer->isVisible(), true);

    auto sliderContainer = viewer->findChild<QWidget *>(QStringLiteral("slidercontainer"));
    QVERIFY(sliderContainer);
    QCOMPARE(sliderContainer->isVisible(), false);

    auto colorBar = viewer->findChild<QWidget *>(QStringLiteral("mColorBar"));
    QVERIFY(colorBar);

    auto scandetectionWidget = viewer->findChild<QWidget *>(QStringLiteral("scandetectionwarning"));
    QVERIFY(!scandetectionWidget); // loaded on demand

    auto opensavefilefolderwidget = viewer->findChild<QWidget *>(QStringLiteral("opensavefilefolderwidget"));
    QVERIFY(!opensavefilefolderwidget); // loaded on demand

    QVERIFY(viewer->toggleFixFontAction());
    QVERIFY(viewer->toggleMimePartTreeAction());
    QVERIFY(viewer->selectAllAction());
    QVERIFY(viewer->copyURLAction());
    QVERIFY(viewer->copyAction());
    QVERIFY(viewer->urlOpenAction());
    QVERIFY(viewer->speakTextAction());
    QVERIFY(viewer->copyImageLocation());
    QVERIFY(viewer->viewSourceAction());
    QVERIFY(viewer->findInMessageAction());
    QVERIFY(viewer->saveAsAction());
    QVERIFY(viewer->saveMessageDisplayFormatAction());
    QVERIFY(viewer->resetMessageDisplayFormatAction());
    QVERIFY(viewer->urlClicked().isEmpty());
    QVERIFY(viewer->imageUrlClicked().isEmpty());
    QCOMPARE(viewer->isFixedFont(), false);
    QVERIFY(viewer->shareServiceUrlMenu());
    delete viewer;
}

static const char s_mail1[] =
    "From: Konqui <konqui@kde.org>\n"
    "To: Friends <friends@kde.org>\n"
    "Date: Sun, 21 Mar 1993 23:56:48 -0800 (PST)\n"
    "Subject: Sample message\n"
    "MIME-Version: 1.0\n"
    "Content-type: text/plain; charset=us-ascii\n"
    "\n"
    "\n"
    "This is a test message.\n"
    "\n";

static const char s_mail2[] =
    "From: David Faure <dfaure@example.com>\n"
    "To: Friends <friends@example.com>\n"
    "Date: Sun, 31 Aug 2016 23:56:48 +0200 (CEST)\n"
    "Subject: Second mail\n"
    "MIME-Version: 1.0\n"
    "Content-type: text/plain; charset=\"us-ascii\"\n"
    "\n"
    "\n"
    "This is the second message.\n"
    "\n";

KMime::Message::Ptr createMsg(const char *data)
{
    KMime::Message::Ptr msgPtr(new KMime::Message());
    msgPtr->setContent(QByteArray(data));
    msgPtr->parse();
    return msgPtr;
}

void ViewerTest::shouldDisplayMessage()
{
    MessageViewer::Viewer viewer(nullptr, nullptr, new KActionCollection(this));
    viewer.setMessage(createMsg(s_mail1), MimeTreeParser::Force);
    // not sure what to check, but at least we check it neither crashes nor hangs
    // TODO: retrieve message text and check it
}

void ViewerTest::shouldSwitchToAnotherMessage()
{
    MessageViewer::Viewer viewer(nullptr, nullptr, new KActionCollection(this));
    viewer.setMessage(createMsg(s_mail1), MimeTreeParser::Force);

    viewer.setMessage(createMsg(s_mail2), MimeTreeParser::Force);
}

QTEST_MAIN(ViewerTest)

#include "moc_viewertest.cpp"
