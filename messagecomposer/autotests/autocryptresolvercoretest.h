/*
  SPDX-FileCopyrightText: 2022 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDir>
#include <QObject>

#include <memory>

namespace Kleo
{
class KeyCache;
}

class AutocryptKeyResolverCoreTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();

    void testAutocryptKeyResolver();
    void testAutocryptKeyResolverSkipSender();
    void testAutocryptKeyResolverUnresolved();
    void testAutocryptKeyResolverPreferNormal();
    void testNormalKeyResolver();

private:
    QDir autocryptDir;
    std::shared_ptr<const Kleo::KeyCache> mKeyCache;
};
