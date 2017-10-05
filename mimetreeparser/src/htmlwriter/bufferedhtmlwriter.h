/*
   Copyright (c) 2017 Volker Krause <vkrause@kde.org>

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

#ifndef MIMETREEPARSER_BUFFEREDHTMLWRITER_H
#define MIMETREEPARSER_BUFFEREDHTMLWRITER_H

#include "mimetreeparser_export.h"
#include <mimetreeparser/htmlwriter.h>

#include <QBuffer>
#include <QByteArray>

namespace MimeTreeParser {
/**
 * QBuffer-backed HtmlWriter
 */
class MIMETREEPARSER_EXPORT BufferedHtmlWriter : public HtmlWriter
{
public:
    BufferedHtmlWriter();
    ~BufferedHtmlWriter();

    void begin() override;
    void end() override;
    void reset() override;
    QIODevice *device() const override;

    QByteArray data() const;
    void clear();

    void extraHead(const QString &str) override;
    void embedPart(const QByteArray &contentId, const QString &url) override;

protected:
    QByteArray m_data;

private:
    QBuffer m_buffer;
};
}

#endif // MIMETREEPARSER_BUFFEREDHTMLWRITER_H
