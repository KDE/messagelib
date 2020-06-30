/*
   SPDX-FileCopyrightText: Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MESSAGEVIEWER_TEST_SHOWONLYMIMEPARTTEST_H
#define MESSAGEVIEWER_TEST_SHOWONLYMIMEPARTTEST_H

#include <QObject>

class ShowOnlyMimePartTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testDrawFrame_data();
    void testDrawFrame();

    void testRelated_data();
    void testRelated();
};

#endif
