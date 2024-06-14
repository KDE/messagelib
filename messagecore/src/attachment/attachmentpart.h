/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include <KMime/Headers>

#include <QList>
#include <QMetaType>
#include <QSharedPointer>

class QUrl;
namespace MessageCore
{
/**
 * @short A class that encapsulates an attachment.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 */
class MESSAGECORE_EXPORT AttachmentPart
{
public:
    /**
     * Defines a pointer to an attachment object.
     */
    using Ptr = QSharedPointer<AttachmentPart>;

    /**
     * Defines a list of pointers to attachment objects.
     */
    using List = QList<Ptr>;

    /**
     * Creates a new attachment part.
     */
    AttachmentPart();

    /**
     * Destroys the attachment part.
     */
    ~AttachmentPart();

    /**
     * Sets the @p name of the attachment.
     *
     * The name will be used in the 'name=' part of
     * the Content-Type header.
     */
    void setName(const QString &name);

    /**
     * Returns the name of the attachment.
     */
    [[nodiscard]] QString name() const;

    /**
     * Sets the file @p name of the attachment.
     *
     * The name will be used in the 'filename=' part of
     * the Content-Disposition header.
     */
    void setFileName(const QString &name);

    /**
     * Returns the file name of the attachment.
     */
    [[nodiscard]] QString fileName() const;

    /**
     * Sets the @p description of the attachment.
     */
    void setDescription(const QString &description);

    /**
     * Returns the description of the attachment.
     */
    [[nodiscard]] QString description() const;

    /**
     * Sets whether the attachment will be displayed inline the message.
     */
    void setInline(bool value);

    /**
     * Returns whether the attachment will be displayed inline the message.
     */
    [[nodiscard]] bool isInline() const;

    /**
     * Sets whether encoding of the attachment will be determined automatically.
     */
    void setAutoEncoding(bool enabled);

    /**
     * Returns whether encoding of the attachment will be determined automatically.
     */
    [[nodiscard]] bool isAutoEncoding() const;

    /**
     * Sets the @p encoding that will be used for the attachment.
     *
     * @note only applies if isAutoEncoding is @c false
     */
    void setEncoding(KMime::Headers::contentEncoding encoding);

    /**
     * Returns the encoding that will be used for the attachment.
     */
    [[nodiscard]] KMime::Headers::contentEncoding encoding() const;

    /**
     * Sets the @p charset that will be used for the attachment.
     */
    void setCharset(const QByteArray &charset);

    /**
     * Returns the charset that will be used for the attachment.
     */
    [[nodiscard]] QByteArray charset() const;

    /**
     * Sets the @p mimeType of the attachment.
     */
    void setMimeType(const QByteArray &mimeType);

    /**
     * Returns the mime type of the attachment.
     */
    [[nodiscard]] QByteArray mimeType() const;

    /**
     * Sets whether the attachment is @p compressed.
     */
    void setCompressed(bool compressed);

    /**
     * Returns whether the attachment is compressed.
     */
    [[nodiscard]] bool isCompressed() const;

    /**
     * Sets whether the attachment is @p encrypted.
     */
    void setEncrypted(bool encrypted);

    /**
     * Returns whether the attachment is encrypted.
     */
    [[nodiscard]] bool isEncrypted() const;

    /**
     * Sets whether the attachment is @p signed.
     */
    void setSigned(bool sign);

    /**
     * Returns whether the attachment is signed.
     */
    [[nodiscard]] bool isSigned() const;

    /**
     * Sets the payload @p data of the attachment.
     */
    void setData(const QByteArray &data);

    /**
     * Returns the payload data of the attachment.
     */
    [[nodiscard]] QByteArray data() const;

    /**
     * Returns the size of the attachment.
     */
    [[nodiscard]] qint64 size() const;

    /**
     * Returns whether the specified attachment part is an encapsulated message
     * (message/rfc822) or a collection of encapsulated messages (multipart/digest)
     */
    [[nodiscard]] bool isMessageOrMessageCollection() const;

    void setUrl(const QUrl &url);
    [[nodiscard]] QUrl url() const;

private:
    //@cond PRIVATE
    class AttachmentPartPrivate;
    AttachmentPartPrivate *const d;
    //@endcond
};
}

Q_DECLARE_METATYPE(MessageCore::AttachmentPart::Ptr)
