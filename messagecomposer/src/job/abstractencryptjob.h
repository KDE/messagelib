/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QStringList>

#include <gpgme++/key.h>
#include <vector>

/**
 * Simple interface that both EncryptJob and SignEncryptJob implement
 * so the composer can extract some encryption-specific job info from them
 */
namespace MessageComposer
{
class AbstractEncryptJob
{
public:
    virtual ~AbstractEncryptJob() = default;

    /**
     * Set the list of encryption keys that should be used.
     */
    virtual void setEncryptionKeys(const std::vector<GpgME::Key> &keys) = 0;

    /**
     * Set the recipients that this message should be encrypted to.
     */
    virtual void setRecipients(const QStringList &rec) = 0;

    virtual std::vector<GpgME::Key> encryptionKeys() const = 0;
    virtual QStringList recipients() const = 0;
};
}
