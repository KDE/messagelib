/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QDir>
#include <QObject>

class AutocryptUtilsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void test_header();
    void test_gossip();
    void test_gossipIgnoreNonExcrypted();
    void test_draft();
    void test_report();
    void test_multiple_headers();
    void test_multiple_headers_invalid();
    void test_multiple_headers_invalid_from();
    void test_multiple_from();
    void test_non_autocrypt();
    void test_update_autocrypt();
    void test_update_non_autocrypt();
    void test_update_autocrypt_gossip();

private:
    QDir baseDir;
};
