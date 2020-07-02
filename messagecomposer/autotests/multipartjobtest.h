/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MULTIPARTJOBTEST_H
#define MULTIPARTJOBTEST_H

#include <QObject>

class MultipartJobTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testMultipartMixed();
    void test8BitPropagation();
};

#endif
