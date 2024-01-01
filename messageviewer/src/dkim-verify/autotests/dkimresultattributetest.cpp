/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimresultattributetest.h"
#include "dkim-verify/dkimresultattribute.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMResultAttributeTest)
DKIMResultAttributeTest::DKIMResultAttributeTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMResultAttributeTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMResultAttribute attr;
    QCOMPARE(attr.error(), -1);
    QCOMPARE(attr.warning(), -1);
    QCOMPARE(attr.status(), -1);
}

void DKIMResultAttributeTest::shouldCloneAttribute()
{
    MessageViewer::DKIMResultAttribute attr;
    int error = 28;
    int warning = 53;
    int status = 5;
    attr.setError(error);
    attr.setWarning(warning);
    attr.setStatus(status);
    MessageViewer::DKIMResultAttribute *cloneAttr = attr.clone();
    QCOMPARE(attr.error(), cloneAttr->error());
    QCOMPARE(attr.warning(), cloneAttr->warning());
    QCOMPARE(attr.status(), cloneAttr->status());
    delete cloneAttr;
}

void DKIMResultAttributeTest::shouldHaveType()
{
    MessageViewer::DKIMResultAttribute attr;
    QCOMPARE(attr.type(), QByteArray("DKIMResultAttribute"));
}

void DKIMResultAttributeTest::shouldDeserializeValue()
{
    MessageViewer::DKIMResultAttribute attr;
    int error = 28;
    int warning = 53;
    int status = 5;
    attr.setError(error);
    attr.setWarning(warning);
    attr.setStatus(status);
    const QByteArray ba = attr.serialized();
    MessageViewer::DKIMResultAttribute result;
    result.deserialize(ba);
    QVERIFY(result == attr);
}

#include "moc_dkimresultattributetest.cpp"
