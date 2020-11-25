/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "bufferedhtmlwriter.h"

using namespace MessageViewer;

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

void BufferedHtmlWriter::setExtraHead(const QString &str)
{
    Q_UNUSED(str)
}

void BufferedHtmlWriter::embedPart(const QByteArray &contentId, const QString &url)
{
    Q_UNUSED(contentId)
    Q_UNUSED(url)
}

void BufferedHtmlWriter::setStyleBody(const QString &styleBody)
{
    Q_UNUSED(styleBody)
}

QByteArray BufferedHtmlWriter::data() const
{
    return m_data;
}

void BufferedHtmlWriter::clear()
{
    m_data.clear();
}
