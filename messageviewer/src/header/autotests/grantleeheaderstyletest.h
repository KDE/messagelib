/*
   SPDX-FileCopyrightText: 2018 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include <QObject>

class GrantleeHeaderStyleTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

private Q_SLOTS:
    void testName();
    void testRenderHeaderNoMessage();
    void testRenderHeaderInvalidTheme();
    void testRenderHeaderEmpty();
    void testRenderHeaderVCard();
    void testRenderHeader_data();
    void testRenderHeader();

private:
    QString expectedDataLocation;
};
