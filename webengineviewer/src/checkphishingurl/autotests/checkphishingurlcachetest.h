/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHECKPHISHINGURLCACHETEST_H
#define CHECKPHISHINGURLCACHETEST_H

#include <QObject>

class CheckPhishingUrlCacheTest : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlCacheTest(QObject *parent = nullptr);
    ~CheckPhishingUrlCacheTest();
private Q_SLOTS:
    void shouldNotBeAMalware();

    void shouldAddValue_data();
    void shouldAddValue();

    void shouldStoreValues();
};

#endif // CHECKPHISHINGURLCACHETEST_H
