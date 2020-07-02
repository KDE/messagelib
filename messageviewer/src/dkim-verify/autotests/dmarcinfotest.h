/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DMARCINFOTEST_H
#define DMARCINFOTEST_H

#include <QObject>

class DMARCInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit DMARCInfoTest(QObject *parent = nullptr);
    ~DMARCInfoTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestExtractDkimKeyRecord_data();
    void shouldTestExtractDkimKeyRecord();
};

#endif // DMARCINFOTEST_H
