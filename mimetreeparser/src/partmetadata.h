/*  -*- c++ -*-
    partmetadata.h

    KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2002-2003 Karl -Heinz Zimmer <khz@kde.org>
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

#include <gpgme++/context.h>
#include <gpgme++/verificationresult.h>

#include <QDateTime>
#include <QStringList>

namespace MimeTreeParser
{
class PartMetaData
{
public:
    PartMetaData()
        : sigSummary(GpgME::Signature::None)
        , isSigned(false)
        , isGoodSignature(false)
        , isEncrypted(false)
        , isDecryptable(false)
        , inProgress(false)
        , technicalProblem(false)
        , isEncapsulatedRfc822Message(false)
    {
    }

    GpgME::Signature::Summary sigSummary;
    QString signClass;
    QString signer;
    QStringList signerMailAddresses;
    QByteArray keyId;
    GpgME::Signature::Validity keyTrust;
    QString status; // to be used for unknown plug-ins
    int status_code; // to be used for i18n of OpenPGP and S/MIME CryptPlugs
    QString errorText;
    QDateTime creationTime;
    QString decryptionError;
    QString auditLog;
    GpgME::Error auditLogError;
    bool isSigned : 1;
    bool isGoodSignature : 1;
    bool isEncrypted : 1;
    bool isDecryptable : 1;
    bool inProgress : 1;
    bool technicalProblem : 1;
    bool isEncapsulatedRfc822Message : 1;
};
}

