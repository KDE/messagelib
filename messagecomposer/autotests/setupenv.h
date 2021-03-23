/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <gpgme++/key.h>

#include <KMime/Message>

namespace MessageComposer
{
namespace Test
{
/**
 * setup a environment variables for tests:
 * * set LC_ALL to C
 * * set KDEHOME
 * * verify that Kleo has correctly loaded all backends
 */
void setupEnv();

/**
 * Returns list of keys used in various crypto routines
 */
std::vector<GpgME::Key> getKeys(bool smime = false);

/**
 * Loads a message from filename and returns a message pointer
 */
KMime::Message::Ptr loadMessageFromFile(const QString &filename);

/**
 * compares a KMime::Content against the referenceFile
 * If the files are not euqal print diff output.
 */
void compareFile(KMime::Content *content, const QString &referenceFile);

/**
 * compare two mails via files.
 * If the files are not euqal print diff output.
 */
void compareFile(const QString &outFile, const QString &referenceFile);

/**
 * Adds keydata into keyring insinde gnupgHome.
 */
void populateKeyring(const QString &gnupgHome, const QByteArray &keydata);

/**
 * Adds keydata into keyring insinde gnupgHome.
 */
void populateKeyring(const QString &gnupgHome, const GpgME::Key &key);

}
}

