/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cryptobodypartmemento.h"
#include <QStringList>
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

namespace MimeTreeParser
{
class VerifyDetachedBodyPartMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    explicit VerifyDetachedBodyPartMemento(QGpgME::VerifyDetachedJob *job, QGpgME::KeyListJob *klj, const QByteArray &signature, const QByteArray &plainText);
    ~VerifyDetachedBodyPartMemento() override;

    [[nodiscard]] bool start() override;
    void exec() override;

    const GpgME::VerificationResult &verifyResult() const
    {
        return m_vr;
    }

    const GpgME::Key &signingKey() const
    {
        return m_key;
    }

private:
    void slotResult(const GpgME::VerificationResult &vr);
    void slotKeyListJobDone();
    void slotNextKey(const GpgME::Key &);

    void saveResult(const GpgME::VerificationResult &);
    [[nodiscard]] bool canStartKeyListJob() const;
    [[nodiscard]] QStringList keyListPattern() const;
    [[nodiscard]] bool startKeyListJob();

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
