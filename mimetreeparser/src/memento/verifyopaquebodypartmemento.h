/*
  Copyright (c) 2014-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MIMETREEPARSER_VERIFYOPAQUEBODYPARTMEMENTO_H
#define MIMETREEPARSER_VERIFYOPAQUEBODYPARTMEMENTO_H

#include "cryptobodypartmemento.h"
#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/key.h>

#include <QString>
#include <QPointer>

#include "interfaces/bodypart.h"

namespace QGpgME {
class VerifyOpaqueJob;
class KeyListJob;
}

class QStringList;

namespace MimeTreeParser {
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

#endif // MIMETREEPARSER_VERIFYOPAQUEBODYPARTMEMENTO_H
