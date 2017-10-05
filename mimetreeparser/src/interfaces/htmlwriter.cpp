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

#include "htmlwriter.h"

#include <QTextStream>

using namespace MimeTreeParser;

HtmlWriter::HtmlWriter() = default;
HtmlWriter::~HtmlWriter() = default;

void HtmlWriter::begin()
{
    if (!m_stream) {
        m_stream.reset(new QTextStream());
    }
    m_stream->setDevice(device());
    m_stream->setCodec("UTF-8");
}

void HtmlWriter::write(const QString &html)
{
    Q_ASSERT(m_stream);
    if (!m_stream) {
        return;
    }
    *stream() << html;
}

void HtmlWriter::end()
{
    if (!m_stream) {
        return;
    }
    m_stream->flush();
    m_stream->setDevice(nullptr);
}

void HtmlWriter::reset()
{
    if (!m_stream) {
        return;
    }
    m_stream->setDevice(nullptr);
}

QTextStream *HtmlWriter::stream() const
{
    return m_stream.get();
}
