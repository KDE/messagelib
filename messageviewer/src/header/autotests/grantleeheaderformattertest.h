/*
   SPDX-FileCopyrightText: 2018 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#ifndef GRANTLEEHEADERFORMATTERTEST_H
#define GRANTLEEHEADERFORMATTERTEST_H

#include <QObject>

class GrantleeHeaderFormatterTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testInvalid();
    void testPrint();
    void testFancyDate();
    void testBlock_data();
    void testBlock();
};

#endif
