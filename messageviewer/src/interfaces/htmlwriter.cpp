/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "htmlwriter.h"

#include <QTextStream>

using namespace MessageViewer;

HtmlWriter::HtmlWriter() = default;
HtmlWriter::~HtmlWriter() = default;

void HtmlWriter::begin()
{
    if (!m_stream) {
        m_stream = std::make_unique<QTextStream>();
    }
    m_stream->setDevice(device());
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
