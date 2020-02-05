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
#include "dkimcheckpolicytest.h"
#include "dkim-verify/dkimcheckpolicy.h"
#include <QTest>
#include <QStandardPaths>
QTEST_MAIN(DKIMCheckPolicyTest)

DKIMCheckPolicyTest::DKIMCheckPolicyTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DKIMCheckPolicyTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMCheckPolicy pol;
    QVERIFY(!pol.verifySignatureWhenOnlyTest());
    QVERIFY(!pol.saveDkimResult());
    QCOMPARE(pol.saveKey(), 0);
    QVERIFY(!pol.autogenerateRule());
    QVERIFY(!pol.checkIfEmailShouldBeSigned());
    QVERIFY(!pol.useDMarc());
    QVERIFY(!pol.useDefaultRules());
    QCOMPARE(pol.rsaSha1Policy(), 1);
    QVERIFY(!pol.useRelaxedParsing());
    QVERIFY(!pol.useOnlyAuthenticationResults());
    QCOMPARE(pol.publicRsaTooSmallPolicy(), 1);
}

void DKIMCheckPolicyTest::shouldAssignValues()
{
    MessageViewer::DKIMCheckPolicy pol;
    int mRsaSha1Policy = 7;
    pol.setRsaSha1Policy(mRsaSha1Policy);
    bool mVerifySignatureWhenOnlyTest = true;
    pol.setVerifySignatureWhenOnlyTest(mVerifySignatureWhenOnlyTest);
    bool mSaveDkimResult = true;
    pol.setSaveDkimResult(mSaveDkimResult);
    int mSaveKey = 8;
    pol.setSaveKey(mSaveKey);
    bool mAutogenerateRule = true;
    pol.setAutogenerateRule(mAutogenerateRule);
    bool mCheckIfEmailShouldBeSigned = true;
    pol.setCheckIfEmailShouldBeSigned(mCheckIfEmailShouldBeSigned);
    bool mUseDMarc = true;
    pol.setUseDMarc(mUseDMarc);
    bool mUseDefaultRules = true;
    pol.setUseDefaultRules(mUseDefaultRules);
    bool mRelaxedParsing = true;
    pol.setUseRelaxedParsing(mRelaxedParsing);
    bool mUseOnlyAuthenticationResults = true;
    pol.setUseOnlyAuthenticationResults(mUseOnlyAuthenticationResults);

    QCOMPARE(pol.verifySignatureWhenOnlyTest(), mVerifySignatureWhenOnlyTest);
    QCOMPARE(pol.saveDkimResult(), mSaveDkimResult);
    QCOMPARE(pol.saveKey(), mSaveKey);
    QCOMPARE(pol.autogenerateRule(), mAutogenerateRule);
    QCOMPARE(pol.checkIfEmailShouldBeSigned(), mCheckIfEmailShouldBeSigned);
    QCOMPARE(pol.useDMarc(), mUseDMarc);
    QCOMPARE(pol.useDefaultRules(), mUseDefaultRules);
    QCOMPARE(pol.rsaSha1Policy(), mRsaSha1Policy);
    QCOMPARE(pol.useRelaxedParsing(), mRelaxedParsing);
    QCOMPARE(pol.useOnlyAuthenticationResults(), mUseOnlyAuthenticationResults);
}
