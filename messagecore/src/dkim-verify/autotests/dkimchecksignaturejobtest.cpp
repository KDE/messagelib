/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimchecksignaturejobtest.h"

#include "dkim-verify/dkimchecksignaturejob.h"
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QTimer>

using namespace std::chrono_literals;
using namespace Qt::Literals::StringLiterals;

QTEST_MAIN(DKIMCheckSignatureJobTest)
// #define USE_EXTRA_CHECK 1
DKIMCheckSignatureJobTest::DKIMCheckSignatureJobTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DKIMCheckSignatureJobTest::initTestCase()
{
    qRegisterMetaType<MessageCore::DKIMCheckSignatureJob::CheckSignatureResult>();
}

void DKIMCheckSignatureJobTest::cleanupTestCase()
{
}

void DKIMCheckSignatureJobTest::shouldHaveDefaultValues()
{
    MessageCore::DKIMCheckSignatureJob job;
    QVERIFY(job.dkimValue().isEmpty());
    QVERIFY(job.headerCanonizationResult().isEmpty());
    QVERIFY(job.bodyCanonizationResult().isEmpty());
    QCOMPARE(job.status(), MessageCore::DKIMCheckSignatureJob::DKIMStatus::Unknown);
    QCOMPARE(job.error(), MessageCore::DKIMCheckSignatureJob::DKIMError::Any);
    QCOMPARE(job.warning(), MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any);
}

void DKIMCheckSignatureJobTest::shouldTestMail_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<MessageCore::DKIMCheckSignatureJob::DKIMError>("dkimerror");
    QTest::addColumn<MessageCore::DKIMCheckSignatureJob::DKIMWarning>("dkimwarning");
    QTest::addColumn<MessageCore::DKIMCheckSignatureJob::DKIMStatus>("dkimstatus");
    QTest::addColumn<QString>("dkimdomain");
    QTest::addColumn<QString>("fromEmail");
    QTest::addColumn<QString>("currentPath");

    const QString curPath = QStringLiteral(DKIM_DATA_DIR "/");

    QTest::addRow("dkim2") << u"dkim2.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any << MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any
                           << MessageCore::DKIMCheckSignatureJob::DKIMStatus::Valid << u"kde.org"_s << u"bugzilla_noreply@kde.org"_s << curPath;

    QTest::addRow("notsigned") << u"notsigned.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any
                               << MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any << MessageCore::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned
                               << QString() << u"richard@weickelt.de"_s << curPath;

    QTest::addRow("broken1") << u"broken1.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any << MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageCore::DKIMCheckSignatureJob::DKIMStatus::Valid << u"kde.org"_s << u"null@kde.org"_s << curPath;

    QTest::addRow("broken2") << u"broken2.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any << MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageCore::DKIMCheckSignatureJob::DKIMStatus::Valid << u"kde.org"_s << u"vkrause@kde.org"_s << curPath;

    QTest::addRow("broken3") << u"broken3.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any
                             << MessageCore::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe << MessageCore::DKIMCheckSignatureJob::DKIMStatus::Valid
                             << u"abonnement.radins.com"_s << u"newsletter@abonnement.radins.com"_s << curPath;

    QTest::addRow("broken4") << u"broken4.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any << MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageCore::DKIMCheckSignatureJob::DKIMStatus::Valid << u"kde.org"_s << u"null@kde.org"_s << curPath;

    QTest::addRow("broken5") << u"broken5.mbox"_s << MessageCore::DKIMCheckSignatureJob::DKIMError::Any << MessageCore::DKIMCheckSignatureJob::DKIMWarning::Any
                             << MessageCore::DKIMCheckSignatureJob::DKIMStatus::Valid << u"kde.org"_s << u"noreply@phabricator.kde.org"_s << curPath;

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
    QFETCH(MessageCore::DKIMCheckSignatureJob::DKIMError, dkimerror);
    QFETCH(MessageCore::DKIMCheckSignatureJob::DKIMWarning, dkimwarning);
    QFETCH(MessageCore::DKIMCheckSignatureJob::DKIMStatus, dkimstatus);
    QFETCH(QString, dkimdomain);
    QFETCH(QString, fromEmail);
    QFETCH(QString, currentPath);
    auto msg = new KMime::Message;
    QFile file(currentPath + fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    msg->setContent(file.readAll());
    msg->parse();
    auto job = new MessageCore::DKIMCheckSignatureJob();
    job->setMessage(std::shared_ptr<KMime::Message>(msg));
    MessageCore::DKIMCheckPolicy pol;
    pol.setSaveKey(false);
    job->setPolicy(pol);
    QSignalSpy dkimSignatureSpy(job, &MessageCore::DKIMCheckSignatureJob::result);
    QTimer::singleShot(10ms, job, &MessageCore::DKIMCheckSignatureJob::start);
    QVERIFY(dkimSignatureSpy.wait());
    QCOMPARE(dkimSignatureSpy.count(), 1);
    const auto info = dkimSignatureSpy.at(0).at(0).value<MessageCore::DKIMCheckSignatureJob::CheckSignatureResult>();
    QCOMPARE(info.warning, dkimwarning);
    QCOMPARE(info.error, dkimerror);
    QCOMPARE(info.status, dkimstatus);
    QCOMPARE(info.sdid, dkimdomain);
    QCOMPARE(info.fromEmail, fromEmail);
}

#include "moc_dkimchecksignaturejobtest.cpp"
