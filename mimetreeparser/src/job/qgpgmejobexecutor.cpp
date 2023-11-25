/*
    SPDX-FileCopyrightText: 2008 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "qgpgmejobexecutor.h"
#include "mimetreeparser_debug.h"

#include <QGpgME/DecryptVerifyJob>
#include <QGpgME/ImportJob>
#include <QGpgME/VerifyDetachedJob>
#include <QGpgME/VerifyOpaqueJob>

#include <QEventLoop>

#include <cassert>

using namespace GpgME;
using namespace MimeTreeParser;

QGpgMEJobExecutor::QGpgMEJobExecutor(QObject *parent)
    : QObject(parent)
    , mEventLoop(new QEventLoop(this))
{
    setObjectName(QLatin1StringView("KleoJobExecutor"));
}

GpgME::VerificationResult QGpgMEJobExecutor::exec(QGpgME::VerifyDetachedJob *job, const QByteArray &signature, const QByteArray &signedData)
{
    qCDebug(MIMETREEPARSER_LOG) << "Starting detached verification job";
    connect(job, &QGpgME::VerifyDetachedJob::result, this, qOverload<const GpgME::VerificationResult &>(&QGpgMEJobExecutor::verificationResult));
    GpgME::Error err = job->start(signature, signedData);
    if (err) {
        return VerificationResult(err);
    }
    mEventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    return mVerificationResult;
}

GpgME::VerificationResult QGpgMEJobExecutor::exec(QGpgME::VerifyOpaqueJob *job, const QByteArray &signedData, QByteArray &plainText)
{
    qCDebug(MIMETREEPARSER_LOG) << "Starting opaque verification job";
    connect(job,
            &QGpgME::VerifyOpaqueJob::result,
            this,
            qOverload<const GpgME::VerificationResult &, const QByteArray &>(&QGpgMEJobExecutor::verificationResult));
    GpgME::Error err = job->start(signedData);
    if (err) {
        plainText.clear();
        return VerificationResult(err);
    }
    mEventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    plainText = mData;
    return mVerificationResult;
}

std::pair<GpgME::DecryptionResult, GpgME::VerificationResult>
QGpgMEJobExecutor::exec(QGpgME::DecryptVerifyJob *job, const QByteArray &cipherText, QByteArray &plainText)
{
    qCDebug(MIMETREEPARSER_LOG) << "Starting decryption job";
    connect(job, &QGpgME::DecryptVerifyJob::result, this, &QGpgMEJobExecutor::decryptResult);
    GpgME::Error err = job->start(cipherText);
    if (err) {
        plainText.clear();
        return std::make_pair(DecryptionResult(err), VerificationResult(err));
    }
    mEventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    plainText = mData;
    return std::make_pair(mDecryptResult, mVerificationResult);
}

GpgME::ImportResult QGpgMEJobExecutor::exec(QGpgME::ImportJob *job, const QByteArray &certData)
{
    connect(job, &QGpgME::AbstractImportJob::result, this, &QGpgMEJobExecutor::importResult);
    GpgME::Error err = job->start(certData);
    if (err) {
        return ImportResult(err);
    }
    mEventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    return mImportResult;
}

Error QGpgMEJobExecutor::auditLogError() const
{
    return mAuditLogError;
}

void QGpgMEJobExecutor::verificationResult(const GpgME::VerificationResult &result)
{
    qCDebug(MIMETREEPARSER_LOG) << "Detached verification job finished";
    auto job = qobject_cast<QGpgME::Job *>(sender());
    assert(job);
    mVerificationResult = result;
    mAuditLogError = job->auditLogError();
    mAuditLog = job->auditLogAsHtml();
    mEventLoop->quit();
}

void QGpgMEJobExecutor::verificationResult(const GpgME::VerificationResult &result, const QByteArray &plainText)
{
    qCDebug(MIMETREEPARSER_LOG) << "Opaque verification job finished";
    auto job = qobject_cast<QGpgME::Job *>(sender());
    assert(job);
    mVerificationResult = result;
    mData = plainText;
    mAuditLogError = job->auditLogError();
    mAuditLog = job->auditLogAsHtml();
    mEventLoop->quit();
}

void QGpgMEJobExecutor::decryptResult(const GpgME::DecryptionResult &decryptionresult,
                                      const GpgME::VerificationResult &verificationresult,
                                      const QByteArray &plainText)
{
    qCDebug(MIMETREEPARSER_LOG) << "Decryption job finished";
    auto job = qobject_cast<QGpgME::Job *>(sender());
    assert(job);
    mVerificationResult = verificationresult;
    mDecryptResult = decryptionresult;
    mData = plainText;
    mAuditLogError = job->auditLogError();
    mAuditLog = job->auditLogAsHtml();
    mEventLoop->quit();
}

void QGpgMEJobExecutor::importResult(const GpgME::ImportResult &result)
{
    auto job = qobject_cast<QGpgME::Job *>(sender());
    assert(job);
    mImportResult = result;
    mAuditLogError = job->auditLogError();
    mAuditLog = job->auditLogAsHtml();
    mEventLoop->quit();
}

QString QGpgMEJobExecutor::auditLogAsHtml() const
{
    return mAuditLog;
}

#include "moc_qgpgmejobexecutor.cpp"
