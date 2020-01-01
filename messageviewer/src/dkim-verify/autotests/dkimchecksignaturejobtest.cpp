/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimchecksignaturejobtest.h"
#include "dkim-verify/dkimchecksignaturejob.h"
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
QTEST_MAIN(DKIMCheckSignatureJobTest)
//#define USE_EXTRA_CHECK 1
DKIMCheckSignatureJobTest::DKIMCheckSignatureJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMCheckSignatureJobTest::initTestCase()
{
    mQcaInitializer = new QCA::Initializer(QCA::Practical, 64);
    qRegisterMetaType<MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult>();
}

void DKIMCheckSignatureJobTest::cleanupTestCase()
{
    delete mQcaInitializer;
}

void DKIMCheckSignatureJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMCheckSignatureJob job;
    QVERIFY(job.dkimValue().isEmpty());
    QVERIFY(job.headerCanonizationResult().isEmpty());
    QVERIFY(job.bodyCanonizationResult().isEmpty());
    QCOMPARE(job.status(), MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Unknown);
    QCOMPARE(job.error(), MessageViewer::DKIMCheckSignatureJob::DKIMError::Any);
    QCOMPARE(job.warning(), MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any);
}

void DKIMCheckSignatureJobTest::shouldTestMail_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<MessageViewer::DKIMCheckSignatureJob::DKIMError>("dkimerror");
    QTest::addColumn<MessageViewer::DKIMCheckSignatureJob::DKIMWarning>("dkimwarning");
    QTest::addColumn<MessageViewer::DKIMCheckSignatureJob::DKIMStatus>("dkimstatus");
    QTest::addColumn<QString>("dkimdomain");
    QTest::addColumn<QString>("fromEmail");
    QTest::addColumn<QString>("currentPath");

    const QString curPath = QStringLiteral(DKIM_DATA_DIR "/");

    QTest::addRow("dkim2") << QStringLiteral("dkim2.mbox")
                           << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                           << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                           << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                           << QStringLiteral("kde.org")
                           << QStringLiteral("bugzilla_noreply@kde.org")
                           << curPath;

    QTest::addRow("notsigned") << QStringLiteral("notsigned.mbox")
                               << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                               << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                               << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned
                               << QString()
                               << QStringLiteral("richard@weickelt.de")
                               << curPath;

    QTest::addRow("broken1") << QStringLiteral("broken1.mbox")
                             << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << QStringLiteral("kde.org")
                             << QStringLiteral("null@kde.org")
                             << curPath;

    QTest::addRow("broken2") << QStringLiteral("broken2.mbox")
                             << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << QStringLiteral("kde.org")
                             << QStringLiteral("vkrause@kde.org")
                             << curPath;

    QTest::addRow("broken3") << QStringLiteral("broken3.mbox")
                             << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe
                             << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << QStringLiteral("abonnement.radins.com")
                             << QStringLiteral("newsletter@abonnement.radins.com")
                             << curPath;

    QTest::addRow("broken4") << QStringLiteral("broken4.mbox")
                             << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << QStringLiteral("kde.org")
                             << QStringLiteral("null@kde.org")
                             << curPath;

    QTest::addRow("broken5") << QStringLiteral("broken5.mbox")
                             << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << QStringLiteral("kde.org")
                             << QStringLiteral("noreply@phabricator.kde.org")
                             << curPath;

    //Used for testing some private emails. Disable by default
#ifdef USE_EXTRA_CHECK
//#if __has_include("dkimchecksignaturejobtest-extra.cpp")
#include "dkimchecksignaturejobtest-extra.cpp"
//#endif
#endif
}

void DKIMCheckSignatureJobTest::shouldTestMail()
{
    QFETCH(QString, fileName);
    QFETCH(MessageViewer::DKIMCheckSignatureJob::DKIMError, dkimerror);
    QFETCH(MessageViewer::DKIMCheckSignatureJob::DKIMWarning, dkimwarning);
    QFETCH(MessageViewer::DKIMCheckSignatureJob::DKIMStatus, dkimstatus);
    QFETCH(QString, dkimdomain);
    QFETCH(QString, fromEmail);
    QFETCH(QString, currentPath);
    KMime::Message *msg = new KMime::Message;
    QFile file(currentPath + fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    msg->setContent(file.readAll());
    msg->parse();
    MessageViewer::DKIMCheckSignatureJob *job = new MessageViewer::DKIMCheckSignatureJob();
    job->setMessage(KMime::Message::Ptr(msg));
    MessageViewer::DKIMCheckPolicy pol;
    pol.setSaveKey(false);
    job->setPolicy(pol);
    QSignalSpy dkimSignatureSpy(job, &MessageViewer::DKIMCheckSignatureJob::result);
    QTimer::singleShot(10, job, &MessageViewer::DKIMCheckSignatureJob::start);
    QVERIFY(dkimSignatureSpy.wait());
    QCOMPARE(dkimSignatureSpy.count(), 1);
    const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult info = dkimSignatureSpy.at(0).at(0).value<MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult>();
    QCOMPARE(info.warning, dkimwarning);
    QCOMPARE(info.error, dkimerror);
    QCOMPARE(info.status, dkimstatus);
    QCOMPARE(info.sdid, dkimdomain);
    QCOMPARE(info.fromEmail, fromEmail);
}
