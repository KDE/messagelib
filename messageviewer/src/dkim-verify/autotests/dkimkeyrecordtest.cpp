/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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
