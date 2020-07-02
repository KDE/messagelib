/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SIGNENCRYPTTEST_H
#define SIGNENCRYPTTEST_H

#include <QObject>

class SignEncryptTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testContent_data();
    void testContent();
    void testContentSubjobChained();
    void testHeaders();
};

#endif
