/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMVIEWERMENUTEST_H
#define DKIMVIEWERMENUTEST_H

#include <QObject>

class DKIMViewerMenuTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMViewerMenuTest(QObject *parent = nullptr);
    ~DKIMViewerMenuTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DKIMVIEWERMENUTEST_H
