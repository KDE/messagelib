/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMRULEWIDGETTEST_H
#define DKIMRULEWIDGETTEST_H

#include <QObject>

class DKIMRuleWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMRuleWidgetTest(QObject *parent = nullptr);
    ~DKIMRuleWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMRULEWIDGETTEST_H
