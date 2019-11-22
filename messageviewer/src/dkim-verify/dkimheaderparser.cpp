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

#include "dkimheaderparser.h"

#include <QChar>
#include <QDebug>

using namespace MessageViewer;

DKIMHeaderParser::DKIMHeaderParser()
{
}

DKIMHeaderParser::~DKIMHeaderParser()
{
    mListHeaders.clear();
}

void DKIMHeaderParser::parse()
{
    mWasAlreadyParsed = true;
    if (mHead.isEmpty()) {
        return;
    }
    int cursor = 0;
    while (cursor < mHead.size()) {
        const int headerStart = cursor;
        int endOfFieldBody;
        const MessageViewer::DKIMHeaderParser::Header header = extractHeader(mHead, headerStart, endOfFieldBody);
        if (header.isValid()) {
            mListHeaders << header;
            cursor = endOfFieldBody + 1;
        } else {
            break;
        }
    }
}

MessageViewer::DKIMHeaderParser::Header DKIMHeaderParser::extractHeader(const QByteArray &head, const int headerStart, int &endOfFieldBody)
{
    int startOfFieldBody = head.indexOf(':', headerStart);
    if (startOfFieldBody < 0) {
        return {};
    }

    MessageViewer::DKIMHeaderParser::Header header;
    const char *rawType = head.constData() + headerStart;
    const size_t rawTypeLen = startOfFieldBody - headerStart;

    startOfFieldBody++; //skip the ':'
    if (startOfFieldBody < head.size() - 1 && head[startOfFieldBody] == ' ') {  // skip the space after the ':', if there's any
        startOfFieldBody++;
    }

    bool folded = false;
    endOfFieldBody = findHeaderLineEnd(head, startOfFieldBody, &folded);

    //Store it as lowercase
    header.headerName = QString::fromLatin1(QByteArray::fromRawData(rawType, rawTypeLen)).toLower();
    if (folded) {
        const auto unfoldedBody = unfoldHeader(head.constData() + startOfFieldBody, endOfFieldBody - startOfFieldBody);
        //qDebug() << " unfoldedBody" << unfoldedBody;
        header.headerValue = QString::fromLatin1(unfoldedBody);
    } else {
        const QByteArray ba = QByteArray::fromRawData(head.constData() + startOfFieldBody, endOfFieldBody - startOfFieldBody);
        //qDebug() << " unfoldedBody ba" << ba;
        header.headerValue = QString::fromLatin1(ba);
    }
    return header;
}

QByteArray DKIMHeaderParser::unfoldHeader(const char *header, size_t headerSize)
{
    QByteArray result;
    if (headerSize == 0) {
        return result;
    }

    // unfolding skips characters so result will be at worst headerSize long
    result.reserve(headerSize);

    const char *end = header + headerSize;
    const char *pos = header, *foldBegin = nullptr, *foldMid = nullptr, *foldEnd = nullptr;
    while ((foldMid = strchr(pos, '\n')) && foldMid < end) {
        foldBegin = foldEnd = foldMid;
        // find the first space before the line-break
        while (foldBegin) {
            if (!QChar::isSpace(*(foldBegin - 1))) {
                break;
            }
            --foldBegin;
        }
        // find the first non-space after the line-break
        while (foldEnd <= end - 1) {
            if (QChar::isSpace(*foldEnd)) {
                ++foldEnd;
            } else if (foldEnd && *(foldEnd - 1) == '\n'
                       && *foldEnd == '=' && foldEnd + 2 < (header + headerSize - 1)
                       && ((*(foldEnd + 1) == '0'
                            && *(foldEnd + 2) == '9')
                           || (*(foldEnd + 1) == '2'
                               && *(foldEnd + 2) == '0'))) {
                // bug #86302: malformed header continuation starting with =09/=20
                foldEnd += 3;
            } else {
                break;
            }
        }

        result.append(pos, foldBegin - pos);
        if (foldEnd < end - 1) {
            result += ' ';
        }
        pos = foldEnd;
    }
    if (end > pos) {
        result.append(pos, end - pos);
    }
    return result;
}

QVector<DKIMHeaderParser::Header> DKIMHeaderParser::listHeaders() const
{
    return mListHeaders;
}

bool DKIMHeaderParser::wasAlreadyParsed() const
{
    return mWasAlreadyParsed;
}

void DKIMHeaderParser::setWasAlreadyParsed(bool wasAlreadyParsed)
{
    mWasAlreadyParsed = wasAlreadyParsed;
}

bool DKIMHeaderParser::operator==(const DKIMHeaderParser &other) const
{
    return other.head() == mHead
            && other.listHeaders() == mListHeaders
            && other.wasAlreadyParsed() == mWasAlreadyParsed;
}

QByteArray DKIMHeaderParser::head() const
{
    return mHead;
}

void DKIMHeaderParser::setHead(const QByteArray &head)
{
    mHead = head;
}

int DKIMHeaderParser::findHeaderLineEnd(const QByteArray &src, int &dataBegin, bool *folded)
{
    int end = dataBegin;
    const int len = src.length() - 1;

    if (folded) {
        *folded = false;
    }

    if (dataBegin < 0) {
        // Not found
        return -1;
    }

    if (dataBegin > len) {
        // No data available
        return len + 1;
    }

    // If the first line contains nothing, but the next line starts with a space
    // or a tab, that means a stupid mail client has made the first header field line
    // entirely empty, and has folded the rest to the next line(s).
    if (src.at(end) == '\n' && end + 1 < len
        && (src[end + 1] == ' ' || src[end + 1] == '\t')) {
        // Skip \n and first whitespace
        dataBegin += 2;
        end += 2;
    }

    if (src.at(end) != '\n') {      // check if the header is not empty
        while (true) {
            end = src.indexOf('\n', end + 1);
            if (end == -1 || end == len) {
                // end of string
                break;
            } else if (src[end + 1] == ' ' || src[end + 1] == '\t'
                       || (src[end + 1] == '=' && end + 3 <= len
                           && ((src[end + 2] == '0' && src[end + 3] == '9')
                               || (src[end + 2] == '2' && src[end + 3] == '0')))) {
                // next line is header continuation or starts with =09/=20 (bug #86302)
                if (folded) {
                    *folded = true;
                }
            } else {
                // end of header (no header continuation)
                break;
            }
        }
    }

    if (end < 0) {
        end = len + 1; //take the rest of the string
    }
    return end;
}

QString DKIMHeaderParser::headerType(const QString &str)
{
    for (int i = mListHeaders.count() -1; i >= 0; --i) {
        if (mListHeaders.at(i).headerName == str) {
            DKIMHeaderParser::Header header = mListHeaders.takeAt(i);
            const QString headerValue = header.headerValue;
            return headerValue;
        }
    }
    return {};
}
