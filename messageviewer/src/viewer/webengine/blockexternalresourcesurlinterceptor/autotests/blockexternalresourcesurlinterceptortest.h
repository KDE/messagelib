/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BLOCKEXTERNALRESOURCESURLINTERCEPTORTEST_H
#define BLOCKEXTERNALRESOURCESURLINTERCEPTORTEST_H

#include <QObject>

class BlockExternalResourcesUrlInterceptorTest : public QObject
{
    Q_OBJECT
public:
    explicit BlockExternalResourcesUrlInterceptorTest(QObject *parent = nullptr);
    ~BlockExternalResourcesUrlInterceptorTest() = default;
private Q_SLOTS:
    void shouldIntercept();
};

#endif // BLOCKEXTERNALRESOURCESURLINTERCEPTORTEST_H
