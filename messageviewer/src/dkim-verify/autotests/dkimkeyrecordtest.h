/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMKEYRECORDTEST_H
#define DKIMKEYRECORDTEST_H

#include <QObject>

class DKIMKeyRecordTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMKeyRecordTest(QObject *parent = nullptr);
    ~DKIMKeyRecordTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestExtractDkimKeyRecord();
    void shouldTestExtractDkimKeyRecord_data();
};

#endif // DKIMKEYRECORDTEST_H
