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
    result.signedBy = QStringLiteral("bli");
    result.status = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned;
    result.error = MessageViewer::DKIMCheckSignatureJob::DKIMError::CorruptedBodyHash;
    result.warning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe;
    w.setResult(result);
    QVERIFY(!mLabel->text().isEmpty());
    w.clear();
    QVERIFY(mLabel->text().isEmpty());
    QCOMPARE(w.result(), {});
}

