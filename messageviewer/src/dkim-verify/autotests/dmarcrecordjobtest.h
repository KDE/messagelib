/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DMARCRECORDJOBTEST_H
#define DMARCRECORDJOBTEST_H

#include <QObject>

class DMARCRecordJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DMARCRecordJobTest(QObject *parent = nullptr);
    ~DMARCRecordJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldBeAbleToStart();
};

#endif // DMARCRECORDJOBTEST_H
