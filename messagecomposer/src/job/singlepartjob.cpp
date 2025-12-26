/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/singlepartjob.h"
using namespace Qt::Literals::StringLiterals;

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
    explicit SinglepartJobPrivate(SinglepartJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    bool chooseCTE();

    QByteArray data;
    bool dataEncoded = false;
    std::unique_ptr<KMime::Headers::ContentDescription> contentDescription;
    std::unique_ptr<KMime::Headers::ContentDisposition> contentDisposition;
    std::unique_ptr<KMime::Headers::ContentID> contentID;
    std::unique_ptr<KMime::Headers::ContentTransferEncoding> contentTransferEncoding;
    std::unique_ptr<KMime::Headers::ContentType> contentType;

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
        contentTransferEncoding = std::make_unique<KMime::Headers::ContentTransferEncoding>();
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
    if (d->resultContent) {
        return d->resultContent->contentDescription();
    }
    if (!d->contentDescription) {
        d->contentDescription = std::make_unique<KMime::Headers::ContentDescription>();
    }
    return d->contentDescription.get();
}

KMime::Headers::ContentDisposition *SinglepartJob::contentDisposition()
{
    Q_D(SinglepartJob);
    if (d->resultContent) {
        d->resultContent->contentDisposition();
    }
    if (!d->contentDisposition) {
        d->contentDisposition = std::make_unique<KMime::Headers::ContentDisposition>();
    }
    return d->contentDisposition.get();
}

KMime::Headers::ContentID *SinglepartJob::contentID()
{
    Q_D(SinglepartJob);
    if (d->resultContent) {
        return d->resultContent->contentID();
    }
    if (!d->contentID) {
        d->contentID = std::make_unique<KMime::Headers::ContentID>();
    }
    return d->contentID.get();
}

KMime::Headers::ContentTransferEncoding *SinglepartJob::contentTransferEncoding()
{
    Q_D(SinglepartJob);
    if (d->resultContent) {
        return d->resultContent->contentTransferEncoding();
    }
    if (!d->contentTransferEncoding) {
        d->contentTransferEncoding = std::make_unique<KMime::Headers::ContentTransferEncoding>();
    }
    return d->contentTransferEncoding.get();
}

KMime::Headers::ContentType *SinglepartJob::contentType()
{
    Q_D(SinglepartJob);
    if (d->resultContent) {
        return d->resultContent->contentType();
    }
    if (!d->contentType) {
        d->contentType = std::make_unique<KMime::Headers::ContentType>();
    }
    return d->contentType.get();
}

void SinglepartJob::process()
{
    Q_D(SinglepartJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.
    d->resultContent = std::make_unique<KMime::Content>();

    if (!d->chooseCTE()) {
        Q_ASSERT(error());
        emitResult();
        return;
    }

    // Set headers.
    if (d->contentDescription) {
        d->resultContent->setHeader(std::move(d->contentDescription));
    }
    if (d->contentDisposition) {
        d->resultContent->setHeader(std::move(d->contentDisposition));
    }
    if (d->contentID) {
        d->resultContent->setHeader(std::move(d->contentID));
    }
    Q_ASSERT(d->contentTransferEncoding); // chooseCTE() created it if it didn't exist.
    {
        d->resultContent->setHeader(std::move(d->contentTransferEncoding));
    }
    if (d->contentType) {
        d->resultContent->setHeader(std::move(d->contentType));
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
