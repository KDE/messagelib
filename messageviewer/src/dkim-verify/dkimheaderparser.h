/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QByteArray>
#include <QList>
namespace MessageViewer
{
/**
 * @brief The DKIMHeaderParser class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMHeaderParser
{
private:
    struct Header {
        QString headerName;
        QString headerValue;
        QByteArray codec;
        [[nodiscard]] bool isValid() const
        {
            return !headerName.isEmpty();
        }

        [[nodiscard]] bool operator==(const Header &other) const
        {
            return other.headerName == headerName && other.headerValue == headerValue && other.codec == codec;
        }
    };

public:
    DKIMHeaderParser();
    ~DKIMHeaderParser();
    void parse();
    void setHead(const QByteArray &head);

    [[nodiscard]] QByteArray head() const;

    [[nodiscard]] QString headerType(const QString &str);
    [[nodiscard]] bool wasAlreadyParsed() const;
    void setWasAlreadyParsed(bool wasAlreadyParsed);

    [[nodiscard]] bool operator==(const DKIMHeaderParser &other) const;
    [[nodiscard]] QList<DKIMHeaderParser::Header> listHeaders() const;

private:
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT static int findHeaderLineEnd(const QByteArray &src, int &dataBegin, bool *folded);
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT MessageViewer::DKIMHeaderParser::Header
    extractHeader(const QByteArray &head, const int headerStart, int &endOfFieldBody);
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT static QByteArray unfoldHeader(const char *header, size_t headerSize);
    QByteArray mHead;
    QList<DKIMHeaderParser::Header> mListHeaders;
    bool mWasAlreadyParsed = false;
};
}
