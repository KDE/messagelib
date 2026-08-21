/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class MultipartJobTest : public QObject
{
    Q_OBJECT
public:
    explicit MultipartJobTest(QObject *parent = nullptr);

private Q_SLOTS:
    void testMultipartMixed();
};
