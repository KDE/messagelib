/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMCHECKPOLICYTEST_H
#define DKIMCHECKPOLICYTEST_H

#include <QObject>

class DKIMCheckPolicyTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckPolicyTest(QObject *parent = nullptr);
    ~DKIMCheckPolicyTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldAssignValues();
};

#endif // DKIMCHECKPOLICYTEST_H
