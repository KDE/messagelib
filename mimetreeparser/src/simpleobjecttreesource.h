/*
  SPDX-FileCopyrightText: 2017 Sandro Kanuß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_SIMPLEOBJECTTREESOURCE_H
#define MIMETREEPARSER_SIMPLEOBJECTTREESOURCE_H

#include "mimetreeparser/objecttreesource.h"
#include <QVector>
#include <memory>

namespace MimeTreeParser {
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

    Q_REQUIRED_RESULT bool autoImportKeys() const override;

    const BodyPartFormatterFactory *bodyPartFormatterFactory() override;
    Q_REQUIRED_RESULT bool decryptMessage() const override;
    void setDecryptMessage(bool decryptMessage);

    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;

    void setPreferredMode(MimeTreeParser::Util::HtmlMode mode);
    Q_REQUIRED_RESULT MimeTreeParser::Util::HtmlMode preferredMode() const override;

    const QTextCodec *overrideCodec() override;

private:
    std::unique_ptr<SimpleObjectTreeSourcePrivate> d;
};
}
#endif
