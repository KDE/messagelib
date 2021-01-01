/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERRULESTEST_H
#define DKIMMANAGERRULESTEST_H

#include <QObject>

class DKIMManagerRulesTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerRulesTest(QObject *parent = nullptr);
    ~DKIMManagerRulesTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldAddRules();
    void shouldClearRules();
};

#endif // DKIMMANAGERRULESTEST_H
