/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "dkimcheckpolicytest.h"
#include "dkim-verify/dkimcheckpolicy.h"
#include <QStandardPaths>
#include <QTest>
QTEST_GUILESS_MAIN(DKIMCheckPolicyTest)

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

#include "moc_dkimcheckpolicytest.cpp"
