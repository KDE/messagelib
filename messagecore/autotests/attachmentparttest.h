/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ATTACHMENTPARTTEST_H
#define ATTACHMENTPARTTEST_H

#include <QObject>

class AttachmentPartTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void testApi();
};

#endif
