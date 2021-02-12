/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMRULETEST_H
#define DKIMRULETEST_H

#include <QObject>

class DKIMRuleTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMRuleTest(QObject *parent = nullptr);
    ~DKIMRuleTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMRULETEST_H
