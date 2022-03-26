/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QDir>
#include <QObject>

class AutocryptStorageTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void test_uniqness();
    void test_unknown();
    void test_existing();
    void test_store();
    void test_addRecipient();
    void test_deleteRecipient();
    void test_create_basedir();

private:
    QDir baseDir;
};
