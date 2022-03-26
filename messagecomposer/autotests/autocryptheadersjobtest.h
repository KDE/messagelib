/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace MessageComposer
{
class AutocryptHeadersJob;
}

class AutocryptHeadersJobTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testAutocryptHeader();
    void testContentChained();
    void testAutocryptGossipHeader();
    void testSetGnupgHome();
    void testStripSenderKey();
};
