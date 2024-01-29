/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by various authors.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentcompressjob.h"

#include <KLocalizedString>
#include <KZip>

#include <QBuffer>
#include <QDateTime>
#include <QSharedPointer>
#include <QTimer>

using namespace MessageCore;
static const mode_t archivePerms = S_IFREG | 0644;

class MessageCore::AttachmentCompressJob::AttachmentCompressJobPrivate
{
public:
    AttachmentCompressJobPrivate(AttachmentCompressJob *qq);

    void doStart(); // slot

    AttachmentCompressJob *const q;
    AttachmentPart::Ptr mOriginalPart;
    AttachmentPart::Ptr mCompressedPart;
    bool mCompressedPartLarger = false;
};

AttachmentCompressJob::AttachmentCompressJobPrivate::AttachmentCompressJobPrivate(AttachmentCompressJob *qq)
    : q(qq)
{
}

void AttachmentCompressJob::AttachmentCompressJobPrivate::doStart()
{
    Q_ASSERT(mOriginalPart);
    const QByteArray decoded = mOriginalPart->data();

    QByteArray array;
    QBuffer dev(&array);
    KZip zip(&dev);
    if (!zip.open(QIODevice::WriteOnly)) {
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("Could not initiate attachment compression."));
        q->emitResult();
        return;
    }

    // Compress.
    zip.setCompression(KZip::DeflateCompression);
    QDateTime zipTime = QDateTime::currentDateTime();
    if (!zip.writeFile(mOriginalPart->name(), decoded, archivePerms, QString(/*user*/), QString(/*group*/), zipTime, zipTime, zipTime)) {
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("Could not compress the attachment."));
        q->emitResult();
        return;
    }
    zip.close();
    mCompressedPartLarger = (array.size() >= decoded.size());

    // Create new part.
    Q_ASSERT(mCompressedPart == nullptr);
    mCompressedPart = AttachmentPart::Ptr(new AttachmentPart);
    mCompressedPart->setName(mOriginalPart->name() + QLatin1StringView(".zip")); // TODO not sure name should be .zipped too
    mCompressedPart->setFileName(mOriginalPart->fileName() + QLatin1StringView(".zip"));
    mCompressedPart->setDescription(mOriginalPart->description());
    mCompressedPart->setInline(mOriginalPart->isInline());
    mCompressedPart->setMimeType("application/zip");
    mCompressedPart->setCompressed(true);
    mCompressedPart->setEncrypted(mOriginalPart->isEncrypted());
    mCompressedPart->setSigned(mOriginalPart->isSigned());
    mCompressedPart->setData(array);
    q->emitResult(); // Success.

    // TODO consider adding a copy constructor to AttachmentPart.
}

AttachmentCompressJob::AttachmentCompressJob(const AttachmentPart::Ptr &part, QObject *parent)
    : KJob(parent)
    , d(new AttachmentCompressJobPrivate(this))
{
    d->mOriginalPart = part;
}

AttachmentCompressJob::~AttachmentCompressJob() = default;

void AttachmentCompressJob::start()
{
    QTimer::singleShot(0, this, [this]() {
        d->doStart();
    });
}

const AttachmentPart::Ptr AttachmentCompressJob::originalPart() const
{
    return d->mOriginalPart;
}

void AttachmentCompressJob::setOriginalPart(const AttachmentPart::Ptr &part)
{
    d->mOriginalPart = part;
}

AttachmentPart::Ptr AttachmentCompressJob::compressedPart() const
{
    return d->mCompressedPart;
}

bool AttachmentCompressJob::isCompressedPartLarger() const
{
    return d->mCompressedPartLarger;
}

#include "moc_attachmentcompressjob.cpp"
