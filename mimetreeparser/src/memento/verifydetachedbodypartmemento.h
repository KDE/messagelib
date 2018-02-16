/*
  Copyright (c) 2014-2018 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MIMETREEPARSER_VERIFYDETACHEDBODYPARTMEMENTO_H
#define MIMETREEPARSER_VERIFYDETACHEDBODYPARTMEMENTO_H

#include "cryptobodypartmemento.h"
#include <gpgme++/verificationresult.h>
#include <gpgme++/key.h>

#include <QString>
#include <QPointer>

#include "interfaces/bodypart.h"

namespace QGpgME {
class VerifyDetachedJob;
class KeyListJob;
}

class QStringList;

namespace MimeTreeParser {
class VerifyDetachedBodyPartMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    VerifyDetachedBodyPartMemento(QGpgME::VerifyDetachedJob *job, QGpgME::KeyListJob *klj, const QByteArray &signature, const QByteArray &plainText);
    ~VerifyDetachedBodyPartMemento();

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

#endif // MIMETREEPARSER_VERIFYDETACHEDBODYPARTMEMENTO_H
