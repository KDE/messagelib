/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KEYRESOLVERTEST_H
#define KEYRESOLVERTEST_H

#include <QObject>
#include <QDir>

class KeyResolverTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();

    void testAutocrypt();
private:
    QDir baseDir;
};

#endif
