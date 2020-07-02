/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimwidgetinfotest.h"
#include "dkim-verify/dkimwidgetinfo.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QTest>
#include <MessageViewer/DKIMCheckSignatureJob>

QTEST_MAIN(DKIMWidgetInfoTest)
DKIMWidgetInfoTest::DKIMWidgetInfoTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMWidgetInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMWidgetInfo w;

    QHBoxLayout *mainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    QLabel *mLabel = w.findChild<QLabel *>(QStringLiteral("label"));
    QVERIFY(mLabel);
    QVERIFY(mLabel->text().isEmpty());

    QCOMPARE(w.currentItemId(), -1);
}

void DKIMWidgetInfoTest::shouldClearWidget()
{
    MessageViewer::DKIMWidgetInfo w;
    QLabel *mLabel = w.findChild<QLabel *>(QStringLiteral("label"));
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult result;
    result.fromEmail = QStringLiteral("bla");
    result.sdid = QStringLiteral("bli");
    result.status = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned;
    result.error = MessageViewer::DKIMCheckSignatureJob::DKIMError::CorruptedBodyHash;
    result.warning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe;
    w.setResult(result, -1);
    QVERIFY(!mLabel->text().isEmpty());
    w.clear();
    QVERIFY(mLabel->text().isEmpty());
    QCOMPARE(w.result(), {});
}
