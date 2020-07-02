/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef URLHASHINGTEST_H
#define URLHASHINGTEST_H

#include <QObject>

class UrlHashingTest : public QObject
{
    Q_OBJECT
public:
    explicit UrlHashingTest(QObject *parent = nullptr);
    ~UrlHashingTest();
private Q_SLOTS:
    void shouldCanonicalizeUrl();
    void shouldCanonicalizeUrl_data();

    void shouldGenerateHostPath_data();
    void shouldGenerateHostPath();

    void shouldGenerateHashList_data();
    void shouldGenerateHashList();
};

#endif // URLHASHINGTEST_H
