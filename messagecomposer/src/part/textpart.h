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
/**
 * @brief The TextPart class
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
    explicit TextPart(QObject *parent = nullptr);
    ~TextPart() override;

    // default true
    [[nodiscard]] bool isWordWrappingEnabled() const;
    void setWordWrappingEnabled(bool enabled);
    // default true
    [[nodiscard]] bool warnBadCharset() const;
    void setWarnBadCharset(bool warn);

    [[nodiscard]] QString cleanPlainText() const;
    void setCleanPlainText(const QString &text);
    [[nodiscard]] QString wrappedPlainText() const;
    void setWrappedPlainText(const QString &text);

    [[nodiscard]] bool isHtmlUsed() const;
    [[nodiscard]] QString cleanHtml() const;
    void setCleanHtml(const QString &text);

    [[nodiscard]] bool hasEmbeddedImages() const;
    [[nodiscard]] KPIMTextEdit::ImageList embeddedImages() const;
    void setEmbeddedImages(const KPIMTextEdit::ImageList &images);

Q_SIGNALS:
    void wordWrappingChanged();
    void warnBadCharsetChanged();
    void cleanPlainTextChanged();
    void wrappedPlainTextChanged();
    void cleanHtmlChanged();
    void embeddedImagesChanged();

private:
    class TextPartPrivate;
    std::unique_ptr<TextPartPrivate> const d;
};
} // namespace MessageComposer
