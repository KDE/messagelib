/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ATTACHMENTFROMMIMECONTENTJOBTEST_H
#define ATTACHMENTFROMMIMECONTENTJOBTEST_H

#include <QObject>

// 33-byte class names say I suck?
class AttachmentFromMimeContentJobTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAttachment();
};

#endif
