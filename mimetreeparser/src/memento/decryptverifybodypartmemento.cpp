/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decryptverifybodypartmemento.h"

#include <QGpgME/DecryptVerifyJob>

using namespace QGpgME;
using namespace GpgME;
using namespace MimeTreeParser;

DecryptVerifyBodyPartMemento::DecryptVerifyBodyPartMemento(DecryptVerifyJob *job, const QByteArray &cipherText)
    : CryptoBodyPartMemento()
    , m_cipherText(cipherText)
    , m_job(job)
{
    Q_ASSERT(m_job);
}

DecryptVerifyBodyPartMemento::~DecryptVerifyBodyPartMemento()
{
    if (m_job) {
        m_job->slotCancel();
    }
}

bool DecryptVerifyBodyPartMemento::start()
{
    Q_ASSERT(m_job);
    if (const Error err = m_job->start(m_cipherText)) {
        m_dr = DecryptionResult(err);
        return false;
    }
    connect(m_job.data(), &DecryptVerifyJob::result, this, &DecryptVerifyBodyPartMemento::slotResult);
    setRunning(true);
    return true;
}

void DecryptVerifyBodyPartMemento::exec()
{
    Q_ASSERT(m_job);
    QByteArray plainText;
    setRunning(true);
    const std::pair<DecryptionResult, VerificationResult> p = m_job->exec(m_cipherText, plainText);
    saveResult(p.first, p.second, plainText);
    m_job->deleteLater(); // exec'ed jobs don't delete themselves
    m_job = nullptr;
}

void DecryptVerifyBodyPartMemento::saveResult(const DecryptionResult &dr, const VerificationResult &vr, const QByteArray &plainText)
{
    Q_ASSERT(m_job);
    setRunning(false);
    m_dr = dr;
    m_vr = vr;
    m_plainText = plainText;
    setAuditLog(m_job->auditLogError(), m_job->auditLogAsHtml());
}

void DecryptVerifyBodyPartMemento::slotResult(const DecryptionResult &dr, const VerificationResult &vr, const QByteArray &plainText)
{
    saveResult(dr, vr, plainText);
    m_job = nullptr;
    notify();
}

#include "moc_decryptverifybodypartmemento.cpp"
