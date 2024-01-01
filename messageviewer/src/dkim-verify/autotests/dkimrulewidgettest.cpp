/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimrulewidgettest.h"
#include "dkim-verify/dkimmanagerulescombobox.h"
#include "dkim-verify/dkimrulewidget.h"
#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTest>

QTEST_MAIN(DKIMRuleWidgetTest)
DKIMRuleWidgetTest::DKIMRuleWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMRuleWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMRuleWidget w;
    QVERIFY(!w.rule().isValid());

    auto layout = w.findChild<QFormLayout *>(QStringLiteral("layout"));
    QCOMPARE(layout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mEnabled = w.findChild<QCheckBox *>(QStringLiteral("enabled"));
    QVERIFY(mEnabled);
    QVERIFY(!mEnabled->text().isEmpty());
    QVERIFY(mEnabled->isChecked());

    auto mDomain = w.findChild<QLineEdit *>(QStringLiteral("domain"));
    QVERIFY(mDomain);
    QVERIFY(mDomain->text().isEmpty());
    QVERIFY(mDomain->isClearButtonEnabled());

    auto mListId = w.findChild<QLineEdit *>(QStringLiteral("listid"));
    QVERIFY(mListId);
    QVERIFY(mListId->text().isEmpty());
    QVERIFY(mListId->isClearButtonEnabled());

    auto mFrom = w.findChild<QLineEdit *>(QStringLiteral("from"));
    QVERIFY(mFrom);
    QVERIFY(mFrom->text().isEmpty());
    QVERIFY(mFrom->isClearButtonEnabled());

    auto mSignatureDomainIdentifier = w.findChild<QLineEdit *>(QStringLiteral("signaturedomainidentifier"));
    QVERIFY(mSignatureDomainIdentifier);
    QVERIFY(mSignatureDomainIdentifier->text().isEmpty());
    QVERIFY(mSignatureDomainIdentifier->isClearButtonEnabled());

    auto mRuleType = w.findChild<MessageViewer::DKIMManageRulesComboBox *>(QStringLiteral("ruletype"));
    QVERIFY(mRuleType);

    auto mPriority = w.findChild<QSpinBox *>(QStringLiteral("priority"));
    QVERIFY(mPriority);
    QCOMPARE(mPriority->value(), 1000);
}

#include "moc_dkimrulewidgettest.cpp"
