/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Parts based on KMail code by various authors.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentfromurljob.h"

#include "messagecore_debug.h"
#include <KIO/TransferJob>
#include <KLocalizedString>

#include <QFileInfo>
#include <QMimeDatabase>
#include <QUrlQuery>

using namespace MessageCore;

class MessageCore::AttachmentFromUrlJob::AttachmentLoadJobPrivate
{
public:
    explicit AttachmentLoadJobPrivate(AttachmentFromUrlJob *qq);

    void transferJobData(KIO::Job *job, const QByteArray &jobData);
    void transferJobResult(KJob *job);

    AttachmentFromUrlJob *const q;
    QByteArray mData;
};

AttachmentFromUrlJob::AttachmentLoadJobPrivate::AttachmentLoadJobPrivate(AttachmentFromUrlJob *qq)
    : q(qq)
{
}

void AttachmentFromUrlJob::AttachmentLoadJobPrivate::transferJobData(KIO::Job *job, const QByteArray &jobData)
{
    Q_UNUSED(job)
    mData += jobData;
}

void AttachmentFromUrlJob::AttachmentLoadJobPrivate::transferJobResult(KJob *job)
{
    if (job->error()) {
        // TODO this loses useful stuff from KIO, like detailed error descriptions, causes+solutions,
        // ... use UiDelegate somehow?
        q->setError(job->error());
        q->setErrorText(job->errorString());
        q->emitResult();
        return;
    }

    Q_ASSERT(dynamic_cast<KIO::TransferJob *>(job));
    auto transferJob = static_cast<KIO::TransferJob *>(job);

    // Determine the MIME type and filename of the attachment.
    const QString mimeTypeName = transferJob->mimetype();
    qCDebug(MESSAGECORE_LOG) << "Mimetype is" << mimeTypeName;

    QString fileName = q->url().fileName();
    fileName.replace(QLatin1Char('\n'), QLatin1Char('_'));
    if (fileName.isEmpty()) {
        QMimeDatabase db;
        const auto mimeType = db.mimeTypeForName(mimeTypeName);
        if (mimeType.isValid()) {
            fileName = i18nc("a file called 'unknown.ext'", "unknown%1", mimeType.preferredSuffix());
        } else {
            fileName = i18nc("a file called 'unknown'", "unknown");
        }
    }

    // Create the AttachmentPart.
    Q_ASSERT(q->attachmentPart() == nullptr); // Not created before.

    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    QUrlQuery query(q->url());
    const QString value = query.queryItemValue(QStringLiteral("charset"));
    part->setCharset(value.toLatin1());
    part->setMimeType(mimeTypeName.toLatin1());
    part->setName(fileName);
    part->setFileName(fileName);
    part->setData(mData);
    part->setUrl(q->url());
    q->setAttachmentPart(part);
    q->emitResult(); // Success.
}

AttachmentFromUrlJob::AttachmentFromUrlJob(const QUrl &url, QObject *parent)
    : AttachmentFromUrlBaseJob(url, parent)
    , d(new AttachmentLoadJobPrivate(this))
{
}

AttachmentFromUrlJob::~AttachmentFromUrlJob() = default;

void AttachmentFromUrlJob::doStart()
{
    if (!url().isValid()) {
        setError(KJob::UserDefinedError);
        setErrorText(i18n("\"%1\" not found. Please specify the full path.", url().toDisplayString()));
        emitResult();
        return;
    }

    if (maximumAllowedSize() != -1 && url().isLocalFile()) {
        const qint64 size = QFileInfo(url().toLocalFile()).size();
        if (size > maximumAllowedSize()) {
            setError(KJob::UserDefinedError);
            setErrorText(i18n("You may not attach files bigger than %1. Share it with storage service.", KIO::convertSize(maximumAllowedSize())));
            emitResult();
            return;
        }
    }

    Q_ASSERT(d->mData.isEmpty()); // Not started twice.

    KIO::TransferJob *job = KIO::get(url(), KIO::NoReload, (uiDelegate() ? KIO::DefaultFlags : KIO::HideProgressInfo));
    connect(job, &KIO::TransferJob::result, this, [this](KJob *job) {
        d->transferJobResult(job);
    });
    connect(job, &KIO::TransferJob::data, this, [this](KIO::Job *job, const QByteArray &ba) {
        d->transferJobData(job, ba);
    });
}

#include "moc_attachmentfromurljob.cpp"
