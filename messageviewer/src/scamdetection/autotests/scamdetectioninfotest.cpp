/*
  SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamdetectioninfotest.h"
#include "scamdetection/scamdetectioninfo.h"
#include <QTest>
QTEST_GUILESS_MAIN(ScamDetectionInfoTest)
ScamDetectionInfoTest::ScamDetectionInfoTest(QObject *parent)
    : QObject{parent}
{
}

void ScamDetectionInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::ScamDetectionInfo info;
    QVERIFY(!info.enabled());
    QVERIFY(info.domainOrEmail().isEmpty());
    QVERIFY(!info.isValid());
    QCOMPARE(info.scamChecks(), MessageViewer::ScamDetectionInfo::ScamDetectionField::None);
}

void ScamDetectionInfoTest::shouldMakeValid()
{
    MessageViewer::ScamDetectionInfo info;
    QVERIFY(!info.isValid());
    info.setEnabled(true);
    QVERIFY(!info.isValid());
    info.setDomainOrEmail(QStringLiteral("bla"));
    QVERIFY(info.isValid());
}

#include "moc_scamdetectioninfotest.cpp"
