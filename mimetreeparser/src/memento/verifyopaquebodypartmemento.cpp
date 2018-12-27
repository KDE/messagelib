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

#include "verifyopaquebodypartmemento.h"
#include "mimetreeparser_debug.h"

#include <QGpgME/VerifyOpaqueJob>
#include <QGpgME/KeyListJob>

#include <gpgme++/keylistresult.h>

#include <qstringlist.h>

#include <cassert>

using namespace QGpgME;
using namespace GpgME;
using namespace MimeTreeParser;

VerifyOpaqueBodyPartMemento::VerifyOpaqueBodyPartMemento(VerifyOpaqueJob *job, KeyListJob *klj, const QByteArray &signature)
    : CryptoBodyPartMemento()
    , m_signature(signature)
    , m_job(job)
    , m_keylistjob(klj)
{
    assert(m_job);
}

VerifyOpaqueBodyPartMemento::~VerifyOpaqueBodyPartMemento()
{
    if (m_job) {
        m_job->slotCancel();
    }
    if (m_keylistjob) {
        m_keylistjob->slotCancel();
    }
}

bool VerifyOpaqueBodyPartMemento::start()
{
    assert(m_job);
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento started";
#endif
    if (const Error err = m_job->start(m_signature)) {
        m_vr = VerificationResult(err);
#ifdef DEBUG_SIGNATURE
        qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento stopped with error";
#endif
        return false;
    }
    connect(m_job.data(), &VerifyOpaqueJob::result,
            this, &VerifyOpaqueBodyPartMemento::slotResult);
    setRunning(true);
    return true;
}

void VerifyOpaqueBodyPartMemento::exec()
{
    assert(m_job);
    setRunning(true);
    QByteArray plainText;
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento execed";
#endif
    saveResult(m_job->exec(m_signature, plainText), plainText);
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento after execed";
#endif
    m_job->deleteLater(); // exec'ed jobs don't delete themselves
    m_job = nullptr;
    if (canStartKeyListJob()) {
        std::vector<GpgME::Key> keys;
        m_keylistjob->exec(keyListPattern(), /*secretOnly=*/ false, keys);
        if (!keys.empty()) {
            m_key = keys.back();
        }
    }
    if (m_keylistjob) {
        m_keylistjob->deleteLater();    // exec'ed jobs don't delete themselves
    }
    m_keylistjob = nullptr;
    setRunning(false);
}

bool VerifyOpaqueBodyPartMemento::canStartKeyListJob() const
{
    if (!m_keylistjob) {
        return false;
    }
    const char *const fpr = m_vr.signature(0).fingerprint();
    return fpr && *fpr;
}

QStringList VerifyOpaqueBodyPartMemento::keyListPattern() const
{
    assert(canStartKeyListJob());
    return QStringList(QString::fromLatin1(m_vr.signature(0).fingerprint()));
}

void VerifyOpaqueBodyPartMemento::saveResult(const VerificationResult &vr, const QByteArray &plainText)
{
    assert(m_job);
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento::saveResult called";
#endif
    m_vr = vr;
    m_plainText = plainText;
    setAuditLog(m_job->auditLogError(), m_job->auditLogAsHtml());
}

void VerifyOpaqueBodyPartMemento::slotResult(const VerificationResult &vr, const QByteArray &plainText)
{
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento::slotResult called";
#endif
    saveResult(vr, plainText);
    m_job = nullptr;
    if (canStartKeyListJob() && startKeyListJob()) {
#ifdef DEBUG_SIGNATURE
        qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento: canStartKeyListJob && startKeyListJob";
#endif
        return;
    }
    if (m_keylistjob) {
        m_keylistjob->deleteLater();
    }
    m_keylistjob = nullptr;
    setRunning(false);
    notify();
}

bool VerifyOpaqueBodyPartMemento::startKeyListJob()
{
    assert(canStartKeyListJob());
    if (const GpgME::Error err = m_keylistjob->start(keyListPattern())) {
        return false;
    }
    connect(m_keylistjob.data(), &Job::done, this, &VerifyOpaqueBodyPartMemento::slotKeyListJobDone);
    connect(m_keylistjob.data(), &KeyListJob::nextKey,
            this, &VerifyOpaqueBodyPartMemento::slotNextKey);
    return true;
}

void VerifyOpaqueBodyPartMemento::slotNextKey(const GpgME::Key &key)
{
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento::slotNextKey called";
#endif
    m_key = key;
}

void VerifyOpaqueBodyPartMemento::slotKeyListJobDone()
{
#ifdef DEBUG_SIGNATURE
    qCDebug(MIMETREEPARSER_LOG) << "tokoe: VerifyOpaqueBodyPartMemento::slotKeyListJobDone called";
#endif
    m_keylistjob = nullptr;
    setRunning(false);
    notify();
}
