/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QByteArray>
#include <QVector>
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
        Q_REQUIRED_RESULT bool isValid() const
        {
            return !headerName.isEmpty();
        }

        Q_REQUIRED_RESULT bool operator==(const Header &other) const
        {
            return other.headerName == headerName && other.headerValue == headerValue && other.codec == codec;
        }
    };

public:
    DKIMHeaderParser();
    ~DKIMHeaderParser();
    void parse();
    void setHead(const QByteArray &head);

    Q_REQUIRED_RESULT QByteArray head() const;

    Q_REQUIRED_RESULT QString headerType(const QString &str);
    Q_REQUIRED_RESULT bool wasAlreadyParsed() const;
    void setWasAlreadyParsed(bool wasAlreadyParsed);

    Q_REQUIRED_RESULT bool operator==(const DKIMHeaderParser &other) const;
    Q_REQUIRED_RESULT QVector<DKIMHeaderParser::Header> listHeaders() const;

private:
    Q_REQUIRED_RESULT static int findHeaderLineEnd(const QByteArray &src, int &dataBegin, bool *folded);
    Q_REQUIRED_RESULT MessageViewer::DKIMHeaderParser::Header extractHeader(const QByteArray &head, const int headerStart, int &endOfFieldBody);
    Q_REQUIRED_RESULT static QByteArray unfoldHeader(const char *header, size_t headerSize);
    QByteArray mHead;
    QVector<DKIMHeaderParser::Header> mListHeaders;
    bool mWasAlreadyParsed = false;
};
}
