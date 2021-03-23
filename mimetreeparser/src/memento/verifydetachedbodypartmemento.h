/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cryptobodypartmemento.h"
#include <gpgme++/key.h>
#include <gpgme++/verificationresult.h>

#include <QPointer>
#include <QString>

#include "interfaces/bodypart.h"

namespace QGpgME
{
class VerifyDetachedJob;
class KeyListJob;
}

#include <QStringList>

namespace MimeTreeParser
{
class VerifyDetachedBodyPartMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    explicit VerifyDetachedBodyPartMemento(QGpgME::VerifyDetachedJob *job, QGpgME::KeyListJob *klj, const QByteArray &signature, const QByteArray &plainText);
    ~VerifyDetachedBodyPartMemento() override;

    bool start() override;
    void exec() override;

    const GpgME::VerificationResult &verifyResult() const
    {
        return m_vr;
    }

    const GpgME::Key &signingKey() const
    {
        return m_key;
    }

private Q_SLOTS:
    void slotResult(const GpgME::VerificationResult &vr);
    void slotKeyListJobDone();
    void slotNextKey(const GpgME::Key &);

private:
    void saveResult(const GpgME::VerificationResult &);
    bool canStartKeyListJob() const;
    QStringList keyListPattern() const;
    bool startKeyListJob();

private:
    // input:
    const QByteArray m_signature;
    const QByteArray m_plainText;
    QPointer<QGpgME::VerifyDetachedJob> m_job;
    QPointer<QGpgME::KeyListJob> m_keylistjob;
    // output:
    GpgME::VerificationResult m_vr;
    GpgME::Key m_key;
};
}

