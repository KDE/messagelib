/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SinglepartJobTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testContent();
    void testContentDisposition();
    void testContentID();
    void testContentType();
    void testContentTransferEncoding();
};
