/*
    SPDX-FileCopyrightText: 2026 Angelo Arifi
    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "../src/viewer/webengine/mailwebengineview.h"

#include <KActionCollection>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>

class TestMailWebEngineView : public MessageViewer::MailWebEngineView
{
public:
    using MailWebEngineView::forwardMouseReleaseEvent;
    using MailWebEngineView::MailWebEngineView;
};

class MailWebEngineViewTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void shouldConsumeHandledLinkRelease()
    {
        KActionCollection actions(this);
        TestMailWebEngineView view(&actions);
        const QUrl url(QStringLiteral("https://example.invalid/link"));
        view.setLinkHovered(url);
        QSignalSpy opened(&view, &MessageViewer::MailWebEngineView::openUrl);
        QVERIFY(opened.isValid());

        QMouseEvent release(QEvent::MouseButtonRelease, QPointF(10, 10), QPointF(10, 10), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        release.ignore();
        view.forwardMouseReleaseEvent(&release);
        QCOMPARE(opened.count(), 1);
        QCOMPARE(opened.at(0).at(0).toUrl(), url);
        QVERIFY(release.isAccepted());

        // A separate, deliberate click must still open the link again.
        QMouseEvent secondRelease(QEvent::MouseButtonRelease, QPointF(10, 10), QPointF(10, 10), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        secondRelease.ignore();
        view.forwardMouseReleaseEvent(&secondRelease);
        QCOMPARE(opened.count(), 2);
        QVERIFY(secondRelease.isAccepted());
    }

    void shouldLeaveNonLinkReleaseUnhandled()
    {
        KActionCollection actions(this);
        TestMailWebEngineView view(&actions);
        QSignalSpy opened(&view, &MessageViewer::MailWebEngineView::openUrl);
        QMouseEvent release(QEvent::MouseButtonRelease, QPointF(10, 10), QPointF(10, 10), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        release.ignore();
        view.forwardMouseReleaseEvent(&release);
        QCOMPARE(opened.count(), 0);
        QVERIFY(!release.isAccepted());
    }
};

QTEST_MAIN(MailWebEngineViewTest)
#include "mailwebengineviewtest.moc"
