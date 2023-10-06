/*
    SPDX-FileCopyrightText: 2008 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <gpgme++/decryptionresult.h>
#include <gpgme++/importresult.h>
#include <gpgme++/verificationresult.h>

#include <QObject>

#include <utility>

class QEventLoop;

namespace QGpgME
{
class DecryptVerifyJob;
class ImportJob;
class VerifyDetachedJob;
class VerifyOpaqueJob;
}

namespace MimeTreeParser
{
/**
  Helper class for synchronous execution of Kleo crypto jobs.
*/
class QGpgMEJobExecutor : public QObject
{
    Q_OBJECT
public:
    explicit QGpgMEJobExecutor(QObject *parent = nullptr);

    GpgME::VerificationResult exec(QGpgME::VerifyDetachedJob *job, const QByteArray &signature, const QByteArray &signedData);
    GpgME::VerificationResult exec(QGpgME::VerifyOpaqueJob *job, const QByteArray &signedData, QByteArray &plainText);
    std::pair<GpgME::DecryptionResult, GpgME::VerificationResult> exec(QGpgME::DecryptVerifyJob *job, const QByteArray &cipherText, QByteArray &plainText);
    GpgME::ImportResult exec(QGpgME::ImportJob *job, const QByteArray &certData);

    [[nodiscard]] GpgME::Error auditLogError() const;
    [[nodiscard]] QString auditLogAsHtml() const;

private Q_SLOTS:
    void verificationResult(const GpgME::VerificationResult &result);
    void verificationResult(const GpgME::VerificationResult &result, const QByteArray &plainText);
    void decryptResult(const GpgME::DecryptionResult &decryptionresult, const GpgME::VerificationResult &verificationresult, const QByteArray &plainText);
    void importResult(const GpgME::ImportResult &result);

private:
    QEventLoop *const mEventLoop;
    GpgME::VerificationResult mVerificationResult;
    GpgME::DecryptionResult mDecryptResult;
    GpgME::ImportResult mImportResult;
    QByteArray mData;
    GpgME::Error mAuditLogError;
    QString mAuditLog;
};
}
