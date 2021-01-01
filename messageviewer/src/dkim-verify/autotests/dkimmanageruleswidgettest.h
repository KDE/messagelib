/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERULESWIDGETTEST_H
#define DKIMMANAGERULESWIDGETTEST_H

#include <QObject>

class DKIMManageRulesWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManageRulesWidgetTest(QObject *parent = nullptr);
    ~DKIMManageRulesWidgetTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMMANAGERULESWIDGETTEST_H
