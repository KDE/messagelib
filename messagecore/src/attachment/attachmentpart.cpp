/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentpart.h"

#include <KMime/Content>
#include <KMime/Util>

#include <QUrl>

using namespace MessageCore;

// TODO move to kmime_util?
static qint64 sizeWithEncoding(const QByteArray &data, KMime::Headers::contentEncoding encoding) // local
{
    KMime::Content content;
    content.setBody(data);
    content.contentTransferEncoding()->setEncoding(encoding);

    return content.size();
}

static KMime::Headers::contentEncoding bestEncodingForTypeAndData(const QByteArray &mimeType, const QByteArray &data)
{
    // Choose the best encoding for text/* and message/* attachments, but
    // always use base64 for anything else to prevent CRLF/LF conversions
    // etc. from corrupting the binary data
    if (mimeType.isEmpty() || mimeType.startsWith("text/") || mimeType.startsWith("message/")) {
        auto possibleEncodings = KMime::encodingsForData(data);
        possibleEncodings.removeAll(KMime::Headers::CE8Bit);
        if (!possibleEncodings.isEmpty()) {
            return possibleEncodings.first();
        } else {
            return KMime::Headers::CEquPr; // TODO: maybe some other default?
        }
    } else {
        return KMime::Headers::CEbase64;
    }
}

class MessageCore::AttachmentPart::AttachmentPartPrivate
{
public:
    AttachmentPartPrivate() = default;

    QUrl mUrl;
    QString mName;
    QString mFileName;
    QString mDescription;
    QByteArray mCharset;
    QByteArray mMimeType;
    QByteArray mData;
    KMime::Headers::contentEncoding mEncoding = KMime::Headers::CE7Bit;
    qint64 mSize = -1;
    bool mIsInline = false;
    bool mAutoEncoding = true;
    bool mCompressed = false;
    bool mToEncrypt = false;
    bool mToSign = false;
};

AttachmentPart::AttachmentPart()
    : d(new AttachmentPartPrivate)
{
}

AttachmentPart::~AttachmentPart()
{
    delete d;
}

QString AttachmentPart::name() const
{
    return d->mName;
}

void AttachmentPart::setName(const QString &name)
{
    d->mName = name;
}

QString AttachmentPart::fileName() const
{
    return d->mFileName;
}

void AttachmentPart::setFileName(const QString &name)
{
    d->mFileName = name;
}

QString AttachmentPart::description() const
{
    return d->mDescription;
}

void AttachmentPart::setDescription(const QString &description)
{
    d->mDescription = description;
}

bool AttachmentPart::isInline() const
{
    return d->mIsInline;
}

void AttachmentPart::setInline(bool inl)
{
    d->mIsInline = inl;
}

bool AttachmentPart::isAutoEncoding() const
{
    return d->mAutoEncoding;
}

void AttachmentPart::setAutoEncoding(bool enabled)
{
    d->mAutoEncoding = enabled;

    if (enabled) {
        d->mEncoding = bestEncodingForTypeAndData(d->mMimeType, d->mData);
    }

    d->mSize = sizeWithEncoding(d->mData, d->mEncoding);
}

KMime::Headers::contentEncoding AttachmentPart::encoding() const
{
    return d->mEncoding;
}

void AttachmentPart::setEncoding(KMime::Headers::contentEncoding encoding)
{
    d->mAutoEncoding = false;
    d->mEncoding = encoding;
    d->mSize = sizeWithEncoding(d->mData, d->mEncoding);
}

QByteArray AttachmentPart::charset() const
{
    return d->mCharset;
}

void AttachmentPart::setCharset(const QByteArray &charset)
{
    d->mCharset = charset;
}

QByteArray AttachmentPart::mimeType() const
{
    return d->mMimeType;
}

void AttachmentPart::setMimeType(const QByteArray &mimeType)
{
    d->mMimeType = mimeType;
}

bool AttachmentPart::isCompressed() const
{
    return d->mCompressed;
}

void AttachmentPart::setCompressed(bool compressed)
{
    d->mCompressed = compressed;
}

bool AttachmentPart::isEncrypted() const
{
    return d->mToEncrypt;
}

void AttachmentPart::setEncrypted(bool encrypted)
{
    d->mToEncrypt = encrypted;
}

bool AttachmentPart::isSigned() const
{
    return d->mToSign;
}

void AttachmentPart::setSigned(bool sign)
{
    d->mToSign = sign;
}

QByteArray AttachmentPart::data() const
{
    return d->mData;
}

void AttachmentPart::setData(const QByteArray &data)
{
    d->mData = data;

    if (d->mAutoEncoding) {
        d->mEncoding = bestEncodingForTypeAndData(d->mMimeType, d->mData);
    }

    d->mSize = sizeWithEncoding(d->mData, d->mEncoding);
}

qint64 AttachmentPart::size() const
{
    return d->mSize;
}

bool AttachmentPart::isMessageOrMessageCollection() const
{
    return (mimeType() == QByteArrayLiteral("message/rfc822")) || (mimeType() == QByteArrayLiteral("multipart/digest"));
}

void AttachmentPart::setUrl(const QUrl &url)
{
    d->mUrl = url;
}

QUrl AttachmentPart::url() const
{
    return d->mUrl;
}
