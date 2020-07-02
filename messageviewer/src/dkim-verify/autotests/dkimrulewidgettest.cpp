/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimrulewidgettest.h"
#include "dkim-verify/dkimrulewidget.h"
#include "dkim-verify/dkimmanagerulescombobox.h"
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

    QFormLayout *layout = w.findChild<QFormLayout *>(QStringLiteral("layout"));
    QCOMPARE(layout->contentsMargins(), QMargins(0, 0, 0, 0));

    QCheckBox *mEnabled = w.findChild<QCheckBox *>(QStringLiteral("enabled"));
    QVERIFY(mEnabled);
    QVERIFY(!mEnabled->text().isEmpty());
    QVERIFY(mEnabled->isChecked());

    QLineEdit *mDomain = w.findChild<QLineEdit *>(QStringLiteral("domain"));
    QVERIFY(mDomain);
    QVERIFY(mDomain->text().isEmpty());
    QVERIFY(mDomain->isClearButtonEnabled());

    QLineEdit *mListId = w.findChild<QLineEdit *>(QStringLiteral("listid"));
    QVERIFY(mListId);
    QVERIFY(mListId->text().isEmpty());
    QVERIFY(mListId->isClearButtonEnabled());

    QLineEdit *mFrom = w.findChild<QLineEdit *>(QStringLiteral("from"));
    QVERIFY(mFrom);
    QVERIFY(mFrom->text().isEmpty());
    QVERIFY(mFrom->isClearButtonEnabled());

    QLineEdit *mSignatureDomainIdentifier = w.findChild<QLineEdit *>(QStringLiteral("signaturedomainidentifier"));
    QVERIFY(mSignatureDomainIdentifier);
    QVERIFY(mSignatureDomainIdentifier->text().isEmpty());
    QVERIFY(mSignatureDomainIdentifier->isClearButtonEnabled());

    MessageViewer::DKIMManageRulesComboBox *mRuleType = w.findChild<MessageViewer::DKIMManageRulesComboBox *>(QStringLiteral("ruletype"));
    QVERIFY(mRuleType);

    QSpinBox *mPriority = w.findChild<QSpinBox *>(QStringLiteral("priority"));
    QVERIFY(mPriority);
    QCOMPARE(mPriority->value(), 1000);
}
