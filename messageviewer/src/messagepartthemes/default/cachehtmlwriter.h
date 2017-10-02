/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#ifndef MESSAGEVIEWER_CACHEHTMLWRITER_H
#define MESSAGEVIEWER_CACHEHTMLWRITER_H

#include "messageviewer_export.h"
#include <MimeTreeParser/BufferedHtmlWriter>

#include <QMap>

namespace MessageViewer {

/** @deprecated port to proper streaming API */
class MESSAGEVIEWER_DEPRECATED CacheHtmlWriter : public MimeTreeParser::BufferedHtmlWriter
{
public:
    explicit CacheHtmlWriter(MimeTreeParser::HtmlWriter *baseWriter = nullptr)
        : mBaseWriter(baseWriter)
    {
        BufferedHtmlWriter::begin();
    }
    ~CacheHtmlWriter() = default;

    void embedPart(const QByteArray &contentId, const QString &url) override
    {
        if (mBaseWriter)
            mBaseWriter->embedPart(contentId, url);
        else
            embedParts.insert(contentId, url);
    }

    void extraHead(const QString &extra) override
    {
        if (mBaseWriter)
            mBaseWriter->extraHead(extra);
        else
            head.append(extra);
    }

    QString html()
    {
        BufferedHtmlWriter::end();
        return QString::fromUtf8(data());
    }

    QString head;
    QMap<QByteArray, QString> embedParts;

private:
    MimeTreeParser::HtmlWriter *mBaseWriter = nullptr;
};

}

#endif // MESSAGEVIEWER_CACHEHTMLWRITER_H
