/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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
