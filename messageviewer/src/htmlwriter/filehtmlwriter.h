/*  -*- c++ -*-
    filehtmlwriter.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <MessageViewer/HtmlWriter>

#include <QFile>

namespace MessageViewer
{
/**
 * @brief The FileHtmlWriter class
 */
class MESSAGEVIEWER_EXPORT FileHtmlWriter : public HtmlWriter
{
public:
    explicit FileHtmlWriter(const QString &filename);
    ~FileHtmlWriter() override;

    void begin() override;
    void end() override;
    void reset() override;
    [[nodiscard]] QIODevice *device() const override;
    void embedPart(const QByteArray &contentId, const QString &url) override;
    void setExtraHead(const QString &str) override;
    void setStyleBody(const QString &styleBody) override;

private:
    QFile mFile;
};
} // namespace MessageViewer
