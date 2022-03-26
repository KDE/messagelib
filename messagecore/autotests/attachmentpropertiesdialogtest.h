/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class AttachmentPropertiesDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentPropertiesDialogTest(QObject *parent = nullptr);
private Q_SLOTS:
    void testAttachmentPartReadWrite();
    void testAttachmentPartReadOnly();
    void testAttachmentPartCancel();
    void testMimeContentReadOnly();
};
