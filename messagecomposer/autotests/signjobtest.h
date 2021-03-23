/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>

namespace MessageComposer
{
class SignJob;
}

class SignJobTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testContentDirect();
    void testContentChained();
    void testHeaders();
    void testRecommentationRFC3156();
    void testMixedContent();

    void testProtectedHeaders_data();
    void testProtectedHeaders();

    void testProtectedHeadersOverwrite();

private:
    void checkSignJob(MessageComposer::SignJob *sJob);
};

