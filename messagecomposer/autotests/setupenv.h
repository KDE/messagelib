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
 * QStandardPaths::setTestModeEnabled(true)
 */
void setupEnv();

/**
 * setup a full environment variables for tests:
 * setupEnv
 * set LC_ALL to en_US.UTF-8
 * set TZ to UTC
 */
void setupFullEnv();

/**
 * Returns list of keys used in various crypto routines
 */
std::vector<GpgME::Key> getKeys(bool smime = false);

/**
 * Loads a message from filename and returns a message pointer
 */
KMime::Message::Ptr loadMessage(const QString &filename);

/**
 * Loads a message from MAIL_DATA_DIR and returns a message pointer
 */
KMime::Message::Ptr loadMessageFromDataDir(const QString &filename);

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
