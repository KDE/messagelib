/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class AutocryptRecipientTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void test_defaults();
    void test_firstAutocryptHeader();
    void test_fromJson();
    void test_fromJsonGossip();
    void test_initiateWithNoAutocryptHeader();
    void test_coreUpdateLogic();
    void test_changedLogic();
    void test_gpgKey();
    void test_setGossipHeader();
    void test_gossipUpdateLogic();
    void test_changedLogicGossip();
    void test_gossipKey();
    void test_getters();
};
