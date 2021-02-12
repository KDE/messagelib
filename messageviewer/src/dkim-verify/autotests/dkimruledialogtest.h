/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMRULEDIALOGTEST_H
#define DKIMRULEDIALOGTEST_H

#include <QObject>

class DKIMRuleDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMRuleDialogTest(QObject *parent = nullptr);
    ~DKIMRuleDialogTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMRULEDIALOGTEST_H
