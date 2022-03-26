/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace MessageComposer
{
class EncryptJob;
}

class EncryptJobTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testContentDirect();
    void testContentChained();
    void testContentSubjobChained();
    void testHeaders();

    void testProtectedHeaders_data();
    void testProtectedHeaders();

    void testSetGnupgHome();

private:
    void checkEncryption(MessageComposer::EncryptJob *eJob);
};
