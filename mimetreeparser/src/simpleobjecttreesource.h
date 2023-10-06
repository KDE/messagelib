/*
  SPDX-FileCopyrightText: 2017 Sandro Kanuß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser/objecttreesource.h"
#include <memory>

namespace MimeTreeParser
{
class SimpleObjectTreeSourcePrivate;

/**
 * A very simple ObjectTreeSource.
 * mostly used in tests and outside the read redering
 */
class MIMETREEPARSER_EXPORT SimpleObjectTreeSource : public Interface::ObjectTreeSource
{
public:
    SimpleObjectTreeSource();
    ~SimpleObjectTreeSource() override;

    [[nodiscard]] bool autoImportKeys() const override;

    const BodyPartFormatterFactory *bodyPartFormatterFactory() override;
    [[nodiscard]] bool decryptMessage() const override;
    void setDecryptMessage(bool decryptMessage);

    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;

    void setPreferredMode(MimeTreeParser::Util::HtmlMode mode);
    [[nodiscard]] MimeTreeParser::Util::HtmlMode preferredMode() const override;

    void setOverrideCodec(QTextCodec *codec);
    const QTextCodec *overrideCodec() override;

    void setOverrideDecoderCodec(QStringDecoder *decoder);
    const QStringDecoder *overrideDecoderCodec() override;

private:
    std::unique_ptr<SimpleObjectTreeSourcePrivate> d;
};
}
