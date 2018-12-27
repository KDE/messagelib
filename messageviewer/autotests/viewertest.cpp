/*
  Copyright (c) 2014-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "viewertest.h"
#include <qtest.h>
#include "messageviewer/viewer.h"
#include <qtestmouse.h>
#include <KActionCollection>

ViewerTest::ViewerTest()
{
}

void ViewerTest::shouldHaveDefaultValuesOnCreation()
{
    MessageViewer::Viewer *viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(
                                                                  this));
    viewer->show();
    QVERIFY(QTest::qWaitForWindowExposed(viewer));

    QWidget *mViewer = viewer->findChild<QWidget *>(QStringLiteral("mViewer"));
    QVERIFY(mViewer);
    QCOMPARE(mViewer->isVisible(), true);

    QWidget *sliderContainer = viewer->findChild<QWidget *>(QStringLiteral("slidercontainer"));
    QVERIFY(sliderContainer);
    QCOMPARE(sliderContainer->isVisible(), false);

    QWidget *colorBar = viewer->findChild<QWidget *>(QStringLiteral("mColorBar"));
    QVERIFY(colorBar);

    QWidget *scandetectionWidget
        = viewer->findChild<QWidget *>(QStringLiteral("scandetectionwarning"));
    QVERIFY(scandetectionWidget);
    QCOMPARE(scandetectionWidget->isVisible(), false);

    QWidget *openattachementfolderwidget
        = viewer->findChild<QWidget *>(QStringLiteral("openattachementfolderwidget"));
    QVERIFY(openattachementfolderwidget);
    QCOMPARE(openattachementfolderwidget->isVisible(), false);

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

static const char s_mail1[]
    = "From: Konqui <konqui@kde.org>\n"
      "To: Friends <friends@kde.org>\n"
      "Date: Sun, 21 Mar 1993 23:56:48 -0800 (PST)\n"
      "Subject: Sample message\n"
      "MIME-Version: 1.0\n"
      "Content-type: text/plain; charset=us-ascii\n"
      "\n"
      "\n"
      "This is a test message.\n"
      "\n";

static const char s_mail2[]
    = "From: David Faure <dfaure@example.com>\n"
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
