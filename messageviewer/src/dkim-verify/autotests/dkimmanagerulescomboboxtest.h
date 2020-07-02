/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERULESCOMBOBOXTEST_H
#define DKIMMANAGERULESCOMBOBOXTEST_H

#include <QObject>

class DKIMManageRulesComboBoxTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManageRulesComboBoxTest(QObject *parent = nullptr);
    ~DKIMManageRulesComboBoxTest() = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldChangeIndex();
};

#endif // DKIMMANAGERULESCOMBOBOXTEST_H
