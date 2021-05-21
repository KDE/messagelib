/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cryptobodypartmemento.h"

#include <gpgme++/decryptionresult.h>
#include <gpgme++/verificationresult.h>

#include <QPointer>

#include "interfaces/bodypart.h"

namespace QGpgME
{
class DecryptVerifyJob;
}

namespace MimeTreeParser
{
class DecryptVerifyBodyPartMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    explicit DecryptVerifyBodyPartMemento(QGpgME::DecryptVerifyJob *job, const QByteArray &cipherText);
    ~DecryptVerifyBodyPartMemento() override;

    Q_REQUIRED_RESULT bool start() override;
    void exec() override;

    const QByteArray &plainText() const
    {
        return m_plainText;
    }

    const GpgME::DecryptionResult &decryptResult() const
    {
        return m_dr;
    }

    const GpgME::VerificationResult &verifyResult() const
    {
        return m_vr;
    }

private Q_SLOTS:
    void slotResult(const GpgME::DecryptionResult &dr, const GpgME::VerificationResult &vr, const QByteArray &plainText);

private:
    void saveResult(const GpgME::DecryptionResult &, const GpgME::VerificationResult &, const QByteArray &);

private:
    // input:
    const QByteArray m_cipherText;
    QPointer<QGpgME::DecryptVerifyJob> m_job;
    // output:
    GpgME::DecryptionResult m_dr;
    GpgME::VerificationResult m_vr;
    QByteArray m_plainText;
};
}
