/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimkeyrecordtest.h"
#include "dkim-verify/dkimkeyrecord.h"
#include <QTest>
QTEST_GUILESS_MAIN(DKIMKeyRecordTest)
DKIMKeyRecordTest::DKIMKeyRecordTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMKeyRecordTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMKeyRecord key;
    QVERIFY(key.version().isEmpty());
    QVERIFY(key.keyType().isEmpty());
    QVERIFY(key.publicKey().isEmpty());
    QVERIFY(key.note().isEmpty());
    QVERIFY(key.service().isEmpty());
    QVERIFY(key.hashAlgorithm().isEmpty());
    QVERIFY(key.flags().isEmpty());
}

void DKIMKeyRecordTest::shouldTestExtractDkimKeyRecord()
{
    QFETCH(QString, dkimstr);
    QFETCH(MessageViewer::DKIMKeyRecord, dkiminforesult);
    QFETCH(bool, isValid);
    MessageViewer::DKIMKeyRecord info;
    QCOMPARE(info.parseKey(dkimstr), isValid);
    if (isValid) {
        const bool isEqual = (info == dkiminforesult);
        if (!isEqual) {
            qDebug() << " info" << info;
            qDebug() << " dkiminforesult" << dkiminforesult;
        }
        QVERIFY(isEqual);
    }
}

void DKIMKeyRecordTest::shouldTestExtractDkimKeyRecord_data()
{
    QTest::addColumn<QString>("dkimstr");
    QTest::addColumn<MessageViewer::DKIMKeyRecord>("dkiminforesult");
    QTest::addColumn<bool>("isValid");
    QTest::addRow("empty") << QString() << MessageViewer::DKIMKeyRecord() << false;
}

#include "moc_dkimkeyrecordtest.cpp"
