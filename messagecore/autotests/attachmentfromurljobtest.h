/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class AttachmentFromUrlJobTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAttachments_data();
    void testAttachments();
    void testAttachmentTooBig();
    void testAttachmentCharset();
};
