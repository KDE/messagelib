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

#include "bufferedhtmlwriter.h"

using namespace MimeTreeParser;

BufferedHtmlWriter::BufferedHtmlWriter()
    : m_buffer(&m_data)
{
}

BufferedHtmlWriter::~BufferedHtmlWriter()
{
    HtmlWriter::end();
}

void BufferedHtmlWriter::begin()
{
    m_buffer.open(QBuffer::WriteOnly);
    HtmlWriter::begin();
}

void BufferedHtmlWriter::end()
{
    HtmlWriter::end();
    m_buffer.close();
}

void BufferedHtmlWriter::reset()
{
    HtmlWriter::reset();
    m_buffer.close();
    clear();
}

QIODevice *BufferedHtmlWriter::device() const
{
    return const_cast<QBuffer *>(&m_buffer);
}

void BufferedHtmlWriter::extraHead(const QString &str)
{
    Q_UNUSED(str);
}

void BufferedHtmlWriter::embedPart(const QByteArray &contentId, const QString &url)
{
    Q_UNUSED(contentId);
    Q_UNUSED(url);
}

QByteArray BufferedHtmlWriter::data() const
{
    return m_data;
}

void BufferedHtmlWriter::clear()
{
    m_data.clear();
}
