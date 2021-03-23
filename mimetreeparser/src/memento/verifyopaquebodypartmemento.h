/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cryptobodypartmemento.h"
#include <gpgme++/decryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/verificationresult.h>

#include <QPointer>
#include <QString>

#include "interfaces/bodypart.h"

namespace QGpgME
{
class VerifyOpaqueJob;
class KeyListJob;
}

#include <QStringList>

namespace MimeTreeParser
{
class VerifyOpaqueBodyPartMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    explicit VerifyOpaqueBodyPartMemento(QGpgME::VerifyOpaqueJob *job, QGpgME::KeyListJob *klj, const QByteArray &signature);
    ~VerifyOpaqueBodyPartMemento() override;

    bool start() override;
    void exec() override;

    const QByteArray &plainText() const
    {
        return m_plainText;
    }

    const GpgME::VerificationResult &verifyResult() const
    {
        return m_vr;
    }

    const GpgME::Key &signingKey() const
    {
        return m_key;
    }

private Q_SLOTS:
    void slotResult(const GpgME::VerificationResult &vr, const QByteArray &plainText);
    void slotKeyListJobDone();
    void slotNextKey(const GpgME::Key &);

private:
    void saveResult(const GpgME::VerificationResult &, const QByteArray &);
    bool canStartKeyListJob() const;
    QStringList keyListPattern() const;
    bool startKeyListJob();

private:
    // input:
    const QByteArray m_signature;
    QPointer<QGpgME::VerifyOpaqueJob> m_job;
    QPointer<QGpgME::KeyListJob> m_keylistjob;
    // output:
    GpgME::VerificationResult m_vr;
    QByteArray m_plainText;
    GpgME::Key m_key;
};
}

