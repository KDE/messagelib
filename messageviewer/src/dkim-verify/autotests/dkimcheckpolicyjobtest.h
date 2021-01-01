/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMCHECKPOLICYJOBTEST_H
#define DKIMCHECKPOLICYJOBTEST_H

#include <QObject>

class DKIMCheckPolicyJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckPolicyJobTest(QObject *parent = nullptr);
    ~DKIMCheckPolicyJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMCHECKPOLICYJOBTEST_H
