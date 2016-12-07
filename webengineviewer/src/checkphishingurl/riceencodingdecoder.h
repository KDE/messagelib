/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#ifndef RICEENCODINGDECODER_H
#define RICEENCODINGDECODER_H

#include "webengineviewer_export.h"
#include "updatedatabaseinfo.h"
namespace WebEngineViewer
{
//https://developers.google.com/safe-browsing/v4/compression
class WEBENGINEVIEWER_EXPORT RiceEncodingDecoder
{
public:
    RiceEncodingDecoder();
    ~RiceEncodingDecoder();

    void setRiceDeltaEncoding(const RiceDeltaEncoding &riceDeltaEncoding);
    bool hasOtherEntries() const;


    static QList<int> decodeRiceIndiceDelta(const WebEngineViewer::RiceDeltaEncoding &riceDeltaEncoding);
    static QByteArray decodeRiceHashesDelta(const WebEngineViewer::RiceDeltaEncoding &riceDeltaEncoding);
private:
};
}

#endif // RICEENCODINGDECODER_H
