/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/singlepartjob.h"

#include "contentjobbase_p.h"
#include "part/globalpart.h"

#include "messagecomposer_debug.h"
#include <KLocalizedString>

#include <KMime/Content>
#include <KMime/Headers>

using namespace MessageComposer;

class MessageComposer::SinglepartJobPrivate : public ContentJobBasePrivate
{
public:
    SinglepartJobPrivate(SinglepartJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    bool chooseCTE();

    QByteArray data;
    bool dataEncoded = false;
    KMime::Headers::ContentDescription *contentDescription = nullptr;
    KMime::Headers::ContentDisposition *contentDisposition = nullptr;
    KMime::Headers::ContentID *contentID = nullptr;
    KMime::Headers::ContentTransferEncoding *contentTransferEncoding = nullptr;
    KMime::Headers::ContentType *contentType = nullptr;

    Q_DECLARE_PUBLIC(SinglepartJob)
};

bool SinglepartJobPrivate::chooseCTE()
{
    Q_Q(SinglepartJob);

    auto allowed = KMime::encodingsForData(data);
    allowed.removeAll(KMime::Headers::CE8Bit);

#if 0 // TODO signing
      // In the following cases only QP and Base64 are allowed:
      // - the buffer will be OpenPGP/MIME signed and it contains trailing
      //   whitespace (cf. RFC 3156)
      // - a line starts with "From "
    if ((willBeSigned && cf.hasTrailingWhitespace())
        || cf.hasLeadingFrom()) {
        ret.removeAll(DwMime::kCte8bit);
        ret.removeAll(DwMime::kCte7bit);
    }
#endif

    if (contentTransferEncoding) {
        // Specific CTE set.  Check that our data fits in it.
        if (!allowed.contains(contentTransferEncoding->encoding())) {
            q->setError(JobBase::BugError);
            q->setErrorText(
                i18n("%1 Content-Transfer-Encoding cannot correctly encode this message.", KMime::nameForEncoding(contentTransferEncoding->encoding())));
            return false;
            // TODO improve error message in case 8bit is requested but not allowed.
        }
    } else {
        // No specific CTE set.  Choose the best one.
        Q_ASSERT(!allowed.isEmpty());
        contentTransferEncoding = new KMime::Headers::ContentTransferEncoding;
        contentTransferEncoding->setEncoding(allowed.first());
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "Settled on encoding" << KMime::nameForEncoding(contentTransferEncoding->encoding());
    return true;
}

SinglepartJob::SinglepartJob(QObject *parent)
    : ContentJobBase(*new SinglepartJobPrivate(this), parent)
{
}

SinglepartJob::~SinglepartJob() = default;

QByteArray SinglepartJob::data() const
{
    Q_D(const SinglepartJob);
    return d->data;
}

void SinglepartJob::setData(const QByteArray &data)
{
    Q_D(SinglepartJob);
    d->data = data;
}

void SinglepartJob::setDataIsEncoded(bool encoded)
{
    Q_D(SinglepartJob);
    d->dataEncoded = encoded;
}

KMime::Headers::ContentDescription *SinglepartJob::contentDescription()
{
    Q_D(SinglepartJob);
    if (!d->contentDescription) {
        d->contentDescription = new KMime::Headers::ContentDescription;
    }
    return d->contentDescription;
}

KMime::Headers::ContentDisposition *SinglepartJob::contentDisposition()
{
    Q_D(SinglepartJob);
    if (!d->contentDisposition) {
        d->contentDisposition = new KMime::Headers::ContentDisposition;
    }
    return d->contentDisposition;
}

KMime::Headers::ContentID *SinglepartJob::contentID()
{
    Q_D(SinglepartJob);
    if (!d->contentID) {
        d->contentID = new KMime::Headers::ContentID;
    }
    return d->contentID;
}

KMime::Headers::ContentTransferEncoding *SinglepartJob::contentTransferEncoding()
{
    Q_D(SinglepartJob);
    if (!d->contentTransferEncoding) {
        d->contentTransferEncoding = new KMime::Headers::ContentTransferEncoding;
    }
    return d->contentTransferEncoding;
}

KMime::Headers::ContentType *SinglepartJob::contentType()
{
    Q_D(SinglepartJob);
    if (!d->contentType) {
        d->contentType = new KMime::Headers::ContentType;
    }
    return d->contentType;
}

void SinglepartJob::process()
{
    Q_D(SinglepartJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.
    d->resultContent = new KMime::Content;

    if (!d->chooseCTE()) {
        Q_ASSERT(error());
        emitResult();
        return;
    }

    // Set headers.
    if (d->contentDescription) {
        d->resultContent->setHeader(d->contentDescription);
    }
    if (d->contentDisposition) {
        d->resultContent->setHeader(d->contentDisposition);
    }
    if (d->contentID) {
        d->resultContent->setHeader(d->contentID);
    }
    Q_ASSERT(d->contentTransferEncoding); // chooseCTE() created it if it didn't exist.
    {
        d->resultContent->setHeader(d->contentTransferEncoding);
    }
    if (d->contentType) {
        d->resultContent->setHeader(d->contentType);
    }

    // Set data.
    if (d->dataEncoded) {
        d->resultContent->setEncodedBody(d->data);
    } else {
        d->resultContent->setBody(d->data);
    }

    emitResult();
}

#include "moc_singlepartjob.cpp"
