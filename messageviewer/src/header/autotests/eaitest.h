/*
   SPDX-FileCopyrightText: 2026 Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class EaiTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRawSourceEncoding();
    void testAddressExtractionEncoding();
};
