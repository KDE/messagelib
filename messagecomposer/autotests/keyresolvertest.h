/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDir>
#include <QObject>

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
