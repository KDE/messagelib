/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMCHECKFULLJOBTEST_H
#define DKIMCHECKFULLJOBTEST_H

#include <QObject>

class DKIMCheckFullJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckFullJobTest(QObject *parent = nullptr);
    ~DKIMCheckFullJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMCHECKFULLJOBTEST_H
