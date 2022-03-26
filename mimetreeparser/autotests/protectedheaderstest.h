/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ProtectedHeadersTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testMailHeaderAsBase_data();
    void testMailHeaderAsBase();

    void testHeaders_data();
    void testHeaders();

    void testMailHeaderAsAddresslist_data();
    void testMailHeaderAsAddresslist();

    void testhasMailHeader_data();
    void testhasMailHeader();

    void testMessagePartsOfMailHeader_data();
    void testMessagePartsOfMailHeader();
};
