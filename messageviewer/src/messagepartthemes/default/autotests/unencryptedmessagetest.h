/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class UnencryptedMessageTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testNotDecrypted_data();
    void testNotDecrypted();
    void testSMimeAutoCertImport();
};
