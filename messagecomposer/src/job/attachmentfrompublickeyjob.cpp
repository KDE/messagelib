/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by:
  Various authors.

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "attachmentfrompublickeyjob.h"

#include <KDialogJobUiDelegate>
#include <KLocalizedString>

#include <QGpgME/ExportJob>
#include <QGpgME/Protocol>

#include <Libkleo/Formatting>
#include <Libkleo/ProgressDialog>

using namespace MessageComposer;
using MessageCore::AttachmentPart;

class MessageComposer::AttachmentFromPublicKeyJob::AttachmentFromPublicKeyJobPrivate
{
public:
    AttachmentFromPublicKeyJobPrivate(AttachmentFromPublicKeyJob *qq);

    void exportResult(const GpgME::Error &error, const QByteArray &keyData); // slot
    void emitGpgError(const GpgME::Error &error);

    AttachmentFromPublicKeyJob *const q;
    GpgME::Key key;
    QByteArray data;
};

AttachmentFromPublicKeyJob::AttachmentFromPublicKeyJobPrivate::AttachmentFromPublicKeyJobPrivate(AttachmentFromPublicKeyJob *qq)
    : q(qq)
{
}

void AttachmentFromPublicKeyJob::AttachmentFromPublicKeyJobPrivate::exportResult(const GpgME::Error &error, const QByteArray &keyData)
{
    if (error) {
        emitGpgError(error);
        return;
    }

    // Create the AttachmentPart.
    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    const QString fingerprint = QLatin1String(key.primaryFingerprint());
    const auto email = Kleo::Formatting::prettyEMail(key);
    if (!email.isEmpty()) {
        // Add email address when available
        part->setName(i18n("OpenPGP Key %1 0x%2", email, fingerprint.right(8)));
        part->setFileName(QStringLiteral("OpenPGP-Key_%1_0x%2.asc").arg(email, fingerprint.right(8)));
    } else {
        part->setName(i18n("OpenPGP Key 0x%1", fingerprint.right(8)));
        part->setFileName(QStringLiteral("OpenPGP-Key_0x%1.asc").arg(fingerprint.right(8)));
    }
    part->setMimeType("application/pgp-keys");
    part->setData(keyData);

    q->setAttachmentPart(part);
    q->emitResult(); // Success.
}

void AttachmentFromPublicKeyJob::AttachmentFromPublicKeyJobPrivate::emitGpgError(const GpgME::Error &error)
{
    Q_ASSERT(error);
    const QString msg = i18n(
        "<p>An error occurred while trying to export "
        "the key from the backend:</p>"
        "<p><b>%1</b></p>",
        QString::fromLocal8Bit(error.asString()));
    q->setError(KJob::UserDefinedError);
    q->setErrorText(msg);
    q->emitResult();
}

AttachmentFromPublicKeyJob::AttachmentFromPublicKeyJob(const GpgME::Key &key, QObject *parent)
    : AttachmentLoadJob(parent)
    , d(new AttachmentFromPublicKeyJobPrivate(this))
{
    d->key = key;
}

AttachmentFromPublicKeyJob::~AttachmentFromPublicKeyJob() = default;

QString AttachmentFromPublicKeyJob::fingerprint() const
{
    return QLatin1String(d->key.primaryFingerprint());
}

GpgME::Key AttachmentFromPublicKeyJob::key() const
{
    return d->key;
}

void AttachmentFromPublicKeyJob::setKey(const GpgME::Key &key)
{
    d->key = key;
}

void AttachmentFromPublicKeyJob::doStart()
{
    QGpgME::ExportJob *job = QGpgME::openpgp()->publicKeyExportJob(true);
    Q_ASSERT(job);
    connect(job, &QGpgME::ExportJob::result, this, [this](const GpgME::Error &error, const QByteArray &ba) {
        d->exportResult(error, ba);
    });

    const GpgME::Error error = job->start(QStringList(fingerprint()));
    if (error) {
        d->emitGpgError(error);
        // TODO check autodeletion policy of Kleo::Jobs...
        return;
    } else if (uiDelegate()) {
        Q_ASSERT(dynamic_cast<KDialogJobUiDelegate *>(uiDelegate()));
        auto delegate = static_cast<KDialogJobUiDelegate *>(uiDelegate());
        (void)new Kleo::ProgressDialog(job, i18n("Exporting key..."), delegate->window());
    }
}

#include "moc_attachmentfrompublickeyjob.cpp"
