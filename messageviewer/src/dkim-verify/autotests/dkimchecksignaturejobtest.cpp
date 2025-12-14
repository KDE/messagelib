/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimchecksignaturejobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimchecksignaturejob.h"
#include <QSignalSpy>
#include <QTest>
#include <QTimer>

using namespace std::chrono_literals;

QTEST_MAIN(DKIMCheckSignatureJobTest)
// #define USE_EXTRA_CHECK 1
DKIMCheckSignatureJobTest::DKIMCheckSignatureJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMCheckSignatureJobTest::initTestCase()
{
    qRegisterMetaType<MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult>();
}

void DKIMCheckSignatureJobTest::cleanupTestCase()
{
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

    QTest::addRow("dkim2") << u"dkim2.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any
                           << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid << u"kde.org"_s << u"bugzilla_noreply@kde.org"_s << curPath;

    QTest::addRow("notsigned") << u"notsigned.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                               << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned
                               << QString() << u"richard@weickelt.de"_s << curPath;

    QTest::addRow("broken1") << u"broken1.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << u"kde.org"_s << u"null@kde.org"_s << curPath;

    QTest::addRow("broken2") << u"broken2.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << u"kde.org"_s << u"vkrause@kde.org"_s << curPath;

    QTest::addRow("broken3") << u"broken3.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe
                             << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid << u"abonnement.radins.com"_s << u"newsletter@abonnement.radins.com"_s
                             << curPath;

    QTest::addRow("broken4") << u"broken4.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << u"kde.org"_s << u"null@kde.org"_s << curPath;

    QTest::addRow("broken5") << u"broken5.mbox"_s << MessageViewer::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageViewer::DKIMCheckSignatureJob::DKIMWarning::Any << MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << u"kde.org"_s << u"noreply@phabricator.kde.org"_s << curPath;

    // Used for testing some private emails. Disable by default
#ifdef USE_EXTRA_CHECK
// #if __has_include("dkimchecksignaturejobtest-extra.cpp")
#include "dkimchecksignaturejobtest-extra.cpp"
// #endif
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
    auto msg = new KMime::Message;
    QFile file(currentPath + fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    msg->setContent(file.readAll());
    msg->parse();
    auto job = new MessageViewer::DKIMCheckSignatureJob();
    job->setMessage(std::shared_ptr<KMime::Message>(msg));
    MessageViewer::DKIMCheckPolicy pol;
    pol.setSaveKey(false);
    job->setPolicy(pol);
    QSignalSpy dkimSignatureSpy(job, &MessageViewer::DKIMCheckSignatureJob::result);
    QTimer::singleShot(10ms, job, &MessageViewer::DKIMCheckSignatureJob::start);
    QVERIFY(dkimSignatureSpy.wait());
    QCOMPARE(dkimSignatureSpy.count(), 1);
    const auto info = dkimSignatureSpy.at(0).at(0).value<MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult>();
    QCOMPARE(info.warning, dkimwarning);
    QCOMPARE(info.error, dkimerror);
    QCOMPARE(info.status, dkimstatus);
    QCOMPARE(info.sdid, dkimdomain);
    QCOMPARE(info.fromEmail, fromEmail);
}

#include "moc_dkimchecksignaturejobtest.cpp"
