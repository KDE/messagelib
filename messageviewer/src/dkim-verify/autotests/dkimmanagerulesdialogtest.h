/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERULESDIALOGTEST_H
#define DKIMMANAGERULESDIALOGTEST_H

#include <QObject>

class DKIMManageRulesDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManageRulesDialogTest(QObject *parent = nullptr);
    ~DKIMManageRulesDialogTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMMANAGERULESDIALOGTEST_H
