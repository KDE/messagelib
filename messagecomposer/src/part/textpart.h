/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QString>

#include "messagecomposer_export.h"
#include "messagepart.h"
#include <KPIMTextEdit/RichTextComposerImages>

namespace MessageComposer
{
/*!
 \class MessageComposer::TextPart
 \inmodule MessageComposer
 \inheaderfile MessageComposer/TextPart

 The TextPart class.
 */
class MESSAGECOMPOSER_EXPORT TextPart : public MessagePart
{
    Q_OBJECT

    /// True iff the text is word wrapped. By default: true.
    Q_PROPERTY(bool isWordWrappingEnabled READ isWordWrappingEnabled WRITE setWordWrappingEnabled NOTIFY wordWrappingChanged)

    /// Default true.
    Q_PROPERTY(bool warnBadCharset READ warnBadCharset WRITE setWarnBadCharset NOTIFY warnBadCharsetChanged)

    Q_PROPERTY(QString cleanPlainText READ cleanPlainText WRITE setCleanPlainText NOTIFY cleanPlainTextChanged)
    Q_PROPERTY(QString wrappedPlainText READ wrappedPlainText WRITE setWrappedPlainText NOTIFY wrappedPlainTextChanged)
    Q_PROPERTY(QString cleanHtml READ cleanHtml WRITE setCleanHtml NOTIFY cleanHtmlChanged)
    Q_PROPERTY(bool isHtmlUsed READ isHtmlUsed NOTIFY cleanHtmlChanged)
    Q_PROPERTY(bool hasEmbeddedImages READ hasEmbeddedImages NOTIFY embeddedImagesChanged)
    Q_PROPERTY(KPIMTextEdit::ImageList embeddedImages READ embeddedImages WRITE setEmbeddedImages NOTIFY embeddedImagesChanged)

public:
    /*! \brief Constructs a TextPart.
        \param parent The parent object.
    */
    explicit TextPart(QObject *parent = nullptr);
    /*! \brief Destroys the TextPart. */
    ~TextPart() override;

    // default true
    /*! \brief Returns whether word wrapping is enabled. */
    [[nodiscard]] bool isWordWrappingEnabled() const;
    /*! \brief Sets whether word wrapping is enabled.
        \param enabled True to enable word wrapping.
    */
    void setWordWrappingEnabled(bool enabled);
    // default true
    /*! \brief Returns whether to warn about bad character sets. */
    [[nodiscard]] bool warnBadCharset() const;
    /*! \brief Sets whether to warn about bad character sets.
        \param warn True to enable the warning.
    */
    void setWarnBadCharset(bool warn);

    /*! \brief Returns the plain text content with cleaned characters. */
    [[nodiscard]] QString cleanPlainText() const;
    /*! \brief Sets the plain text content.
        \param text The plain text to set.
    */
    void setCleanPlainText(const QString &text);
    /*! \brief Returns the plain text content wrapped to the appropriate line length. */
    [[nodiscard]] QString wrappedPlainText() const;
    /*! \brief Sets the wrapped plain text content.
        \param text The wrapped text to set.
    */
    void setWrappedPlainText(const QString &text);

    /*! \brief Returns whether HTML content is being used. */
    [[nodiscard]] bool isHtmlUsed() const;
    /*! \brief Returns the HTML content with cleaned elements. */
    [[nodiscard]] QString cleanHtml() const;
    /*! \brief Sets the HTML content.
        \param text The HTML text to set.
    */
    void setCleanHtml(const QString &text);

    /*! \brief Returns whether the message has embedded images. */
    [[nodiscard]] bool hasEmbeddedImages() const;
    /*! \brief Returns the list of embedded images. */
    [[nodiscard]] KPIMTextEdit::ImageList embeddedImages() const;
    /*! \brief Sets the list of embedded images.
        \param images The list of images to embed.
    */
    void setEmbeddedImages(const KPIMTextEdit::ImageList &images);

Q_SIGNALS:
    /*! \brief Emitted when the word wrapping setting changes. */
    void wordWrappingChanged();
    /*! \brief Emitted when the bad charset warning setting changes. */
    void warnBadCharsetChanged();
    /*! \brief Emitted when the clean plain text content changes. */
    void cleanPlainTextChanged();
    /*! \brief Emitted when the wrapped plain text content changes. */
    void wrappedPlainTextChanged();
    /*! \brief Emitted when the HTML content changes. */
    void cleanHtmlChanged();
    /*! \brief Emitted when the embedded images list changes. */
    void embeddedImagesChanged();

private:
    class TextPartPrivate;
    std::unique_ptr<TextPartPrivate> const d;
};
} // namespace MessageComposer
