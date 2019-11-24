/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "dkimcheckauthenticationstatusjobtest.h"
#include "dkim-verify/dkimcheckauthenticationstatusjob.h"
#include "dkim-verify/dkimauthenticationstatusinfo.h"
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <KMime/Message>

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
}

void DKIMCheckAuthenticationStatusJobTest::shouldTestMail_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("currentPath");
    const QString curPath = QStringLiteral(DKIM_DATA_DIR "/");

    QTest::addRow("dkim2") << QStringLiteral("dkim2.mbox")
                           << curPath;
    QTest::addRow("notsigned") << QStringLiteral("notsigned.mbox")
                               << curPath;
    QTest::addRow("broken1") << QStringLiteral("broken1.mbox")
                             << curPath;
}

void DKIMCheckAuthenticationStatusJobTest::shouldTestMail()
{
    QFETCH(QString, fileName);
    QFETCH(QString, currentPath);
    KMime::Message *msg = new KMime::Message;
    QFile file(currentPath + fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    msg->setContent(file.readAll());
    msg->parse();

    MessageViewer::DKIMCheckAuthenticationStatusJob *job = new  MessageViewer::DKIMCheckAuthenticationStatusJob(this);
    MessageViewer::DKIMHeaderParser mHeaderParser;
    mHeaderParser.setHead(msg->head());
    mHeaderParser.parse();
    job->setHeaderParser(mHeaderParser);
    QSignalSpy dkimSignatureSpy(job, &MessageViewer::DKIMCheckAuthenticationStatusJob::result);
    QTimer::singleShot(10, job, &MessageViewer::DKIMCheckAuthenticationStatusJob::start);
    QVERIFY(dkimSignatureSpy.wait());
}
