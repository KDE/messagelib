/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckauthenticationstatusjobtest.h"
#include "dkim-verify/dkimauthenticationstatusinfo.h"
#include "dkim-verify/dkimcheckauthenticationstatusjob.h"
#include <KMime/Message>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <chrono>

using namespace std::chrono_literals;

QTEST_MAIN(DKIMCheckAuthenticationStatusJobTest)

DKIMCheckAuthenticationStatusJobTest::DKIMCheckAuthenticationStatusJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMCheckAuthenticationStatusJobTest::initTestCase()
{
    qRegisterMetaType<MessageViewer::DKIMAuthenticationStatusInfo>();
}

void DKIMCheckAuthenticationStatusJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMCheckAuthenticationStatusJob job;
    QVERIFY(!job.canStart());
    QVERIFY(!job.useRelaxedParsing());
}

void DKIMCheckAuthenticationStatusJobTest::shouldTestMail_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("currentPath");
    QTest::addColumn<bool>("relaxedParsing");
    const QString curPath = QStringLiteral(DKIM_DATA_DIR "/");

    QTest::addRow("dkim2") << QStringLiteral("dkim2.mbox") << curPath << false;
    QTest::addRow("notsigned") << QStringLiteral("notsigned.mbox") << curPath << false;
    QTest::addRow("broken1") << QStringLiteral("broken1.mbox") << curPath << false;
}

void DKIMCheckAuthenticationStatusJobTest::shouldTestMail()
{
    QFETCH(QString, fileName);
    QFETCH(QString, currentPath);
    QFETCH(bool, relaxedParsing);
    auto msg = new KMime::Message;
    QFile file(currentPath + fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    msg->setContent(file.readAll());
    msg->parse();

    auto job = new MessageViewer::DKIMCheckAuthenticationStatusJob(this);
    MessageViewer::DKIMHeaderParser mHeaderParser;
    mHeaderParser.setHead(msg->head());
    mHeaderParser.parse();
    job->setHeaderParser(mHeaderParser);
    job->setUseRelaxedParsing(relaxedParsing);
    QSignalSpy dkimSignatureSpy(job, &MessageViewer::DKIMCheckAuthenticationStatusJob::result);
    QTimer::singleShot(10ms, job, &MessageViewer::DKIMCheckAuthenticationStatusJob::start);
    QVERIFY(dkimSignatureSpy.wait());
    delete msg;
}

#include "moc_dkimcheckauthenticationstatusjobtest.cpp"
