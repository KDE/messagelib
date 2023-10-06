/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <MessageViewer/HtmlWriter>

#include <QBuffer>
#include <QByteArray>

namespace MessageViewer
{
/**
 * QBuffer-backed HtmlWriter
 */
class MESSAGEVIEWER_EXPORT BufferedHtmlWriter : public HtmlWriter
{
public:
    BufferedHtmlWriter();
    ~BufferedHtmlWriter() override;

    void begin() override;
    void end() override;
    void reset() override;
    [[nodiscard]] QIODevice *device() const override;

    [[nodiscard]] QByteArray data() const;
    void clear();

    void setExtraHead(const QString &str) override;
    void embedPart(const QByteArray &contentId, const QString &url) override;
    void setStyleBody(const QString &styleBody) override;

protected:
    QByteArray m_data;

private:
    QBuffer m_buffer;
};
}
