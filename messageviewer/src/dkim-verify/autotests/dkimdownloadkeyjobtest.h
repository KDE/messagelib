/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMDOWNLOADKEYJOBTEST_H
#define DKIMDOWNLOADKEYJOBTEST_H

#include <QObject>

class DKIMDownloadKeyJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMDownloadKeyJobTest(QObject *parent = nullptr);
    ~DKIMDownloadKeyJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestCanStart();
    void shouldVerifyResolveDns();
};

#endif // DKIMDOWNLOADKEYJOBTEST_H
