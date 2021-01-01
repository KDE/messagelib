/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMINFOTEST_H
#define DKIMINFOTEST_H

#include <QObject>

class DKIMInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMInfoTest(QObject *parent = nullptr);
    ~DKIMInfoTest() = default;

private Q_SLOTS:
    void shouldTestExtractDkimInfo();
    void shouldTestExtractDkimInfo_data();
    void shouldHaveDefaultValue();
};

#endif // DKIMINFOTEST_H
