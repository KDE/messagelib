/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERKEYDIALOGTEST_H
#define DKIMMANAGERKEYDIALOGTEST_H

#include <QObject>

class DKIMManagerKeyDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyDialogTest(QObject *parent = nullptr);
    ~DKIMManagerKeyDialogTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // DKIMMANAGERKEYDIALOGTEST_H
