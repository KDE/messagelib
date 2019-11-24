/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef DKIMHEADERPARSER_H
#define DKIMHEADERPARSER_H

#include "messageviewer_export.h"

#include <QByteArray>
#include <QVector>
namespace MessageViewer {
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
            return other.headerName == headerName
                   && other.headerValue == headerValue
                   && other.codec == codec;
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

    Q_REQUIRED_RESULT int findHeaderLineEnd(const QByteArray &src, int &dataBegin, bool *folded);
    Q_REQUIRED_RESULT MessageViewer::DKIMHeaderParser::Header extractHeader(const QByteArray &head, const int headerStart, int &endOfFieldBody);
    Q_REQUIRED_RESULT QByteArray unfoldHeader(const char *header, size_t headerSize);
    QByteArray mHead;
    QVector<DKIMHeaderParser::Header> mListHeaders;
    bool mWasAlreadyParsed = false;
};
}
#endif // DKIMHEADERPARSER_H
