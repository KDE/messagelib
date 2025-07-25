/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimwidgetinfotest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimwidgetinfo.h"
#include <MessageViewer/DKIMCheckSignatureJob>
#include <QHBoxLayout>
#include <QLabel>
#include <QTest>

QTEST_MAIN(DKIMWidgetInfoTest)
DKIMWidgetInfoTest::DKIMWidgetInfoTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMWidgetInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMWidgetInfo w;

    auto mainLayout = w.findChild<QHBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mLabel = w.findChild<QLabel *>(u"label"_s);
    QVERIFY(mLabel);
    QVERIFY(mLabel->text().isEmpty());

    QCOMPARE(w.currentItemId(), -1);
}

void DKIMWidgetInfoTest::shouldClearWidget()
{
    MessageViewer::DKIMWidgetInfo w;
    auto mLabel = w.findChild<QLabel *>(u"label"_s);
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult result;
    result.fromEmail = u"bla"_s;
    result.sdid = u"bli"_s;
    result.status = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned;
    result.error = MessageViewer::DKIMCheckSignatureJob::DKIMError::CorruptedBodyHash;
    result.warning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe;
    w.setResult(result, -1);
    QVERIFY(!mLabel->text().isEmpty());
    w.clear();
    QVERIFY(mLabel->text().isEmpty());
    QCOMPARE(w.result(), MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult());
}

#include "moc_dkimwidgetinfotest.cpp"
