/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "textpart.h"

using namespace MessageComposer;

class Q_DECL_HIDDEN TextPart::Private
{
public:
    Private()
    {
    }

    KPIMTextEdit::ImageList embeddedImages;
    QString cleanPlainText;
    QString wrappedPlainText;
    QString cleanHtml;
    bool wordWrappingEnabled = true;
    bool warnBadCharset = true;
};

TextPart::TextPart(QObject *parent)
    : MessagePart(parent)
    , d(new Private)
{
}

TextPart::~TextPart()
{
    delete d;
}

bool TextPart::isWordWrappingEnabled() const
{
    return d->wordWrappingEnabled;
}

void TextPart::setWordWrappingEnabled(bool enabled)
{
    d->wordWrappingEnabled = enabled;
}

bool TextPart::warnBadCharset() const
{
    return d->warnBadCharset;
}

void TextPart::setWarnBadCharset(bool warn)
{
    d->warnBadCharset = warn;
}

QString TextPart::cleanPlainText() const
{
    return d->cleanPlainText;
}

void TextPart::setCleanPlainText(const QString &text)
{
    d->cleanPlainText = text;
}

QString TextPart::wrappedPlainText() const
{
    return d->wrappedPlainText;
}

void TextPart::setWrappedPlainText(const QString &text)
{
    d->wrappedPlainText = text;
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
    d->cleanHtml = text;
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
