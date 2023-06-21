/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "textpart.h"

using namespace MessageComposer;

class TextPart::TextPartPrivate
{
public:
    TextPartPrivate() = default;

    KPIMTextEdit::ImageList embeddedImages;
    QString cleanPlainText;
    QString wrappedPlainText;
    QString cleanHtml;
    bool wordWrappingEnabled = true;
    bool warnBadCharset = true;
};

TextPart::TextPart(QObject *parent)
    : MessagePart(parent)
    , d(new TextPartPrivate)
{
}

TextPart::~TextPart() = default;

bool TextPart::isWordWrappingEnabled() const
{
    return d->wordWrappingEnabled;
}

void TextPart::setWordWrappingEnabled(bool enabled)
{
    if (d->wordWrappingEnabled == enabled) {
        return;
    }
    d->wordWrappingEnabled = enabled;
    Q_EMIT wordWrappingChanged();
}

bool TextPart::warnBadCharset() const
{
    return d->warnBadCharset;
}

void TextPart::setWarnBadCharset(bool warn)
{
    if (d->warnBadCharset == warn) {
        return;
    }
    d->warnBadCharset = warn;
    Q_EMIT warnBadCharsetChanged();
}

QString TextPart::cleanPlainText() const
{
    return d->cleanPlainText;
}

void TextPart::setCleanPlainText(const QString &text)
{
    if (d->cleanPlainText == text) {
        return;
    }
    d->cleanPlainText = text;
    Q_EMIT cleanPlainTextChanged();
}

QString TextPart::wrappedPlainText() const
{
    return d->wrappedPlainText;
}

void TextPart::setWrappedPlainText(const QString &text)
{
    if (d->wrappedPlainText == text) {
        return;
    }
    d->wrappedPlainText = text;
    Q_EMIT wrappedPlainTextChanged();
}

bool TextPart::isHtmlUsed() const
{
    return !d->cleanHtml.isEmpty();
}

QString TextPart::cleanHtml() const
{
    return d->cleanHtml;
}

void TextPart::setCleanHtml(const QString &text)
{
    if (d->cleanHtml == text) {
        return;
    }
    d->cleanHtml = text;
    Q_EMIT cleanHtmlChanged();
}

bool TextPart::hasEmbeddedImages() const
{
    return !d->embeddedImages.isEmpty();
}

KPIMTextEdit::ImageList TextPart::embeddedImages() const
{
    return d->embeddedImages;
}

void TextPart::setEmbeddedImages(const KPIMTextEdit::ImageList &images)
{
    d->embeddedImages = images;
}

#include "moc_textpart.cpp"
