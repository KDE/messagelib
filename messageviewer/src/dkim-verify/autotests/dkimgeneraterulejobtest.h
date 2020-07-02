/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMGENERATERULEJOBTEST_H
#define DKIMGENERATERULEJOBTEST_H

#include <QObject>

class DKIMGenerateRuleJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMGenerateRuleJobTest(QObject *parent = nullptr);
    ~DKIMGenerateRuleJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMGENERATERULEJOBTEST_H
