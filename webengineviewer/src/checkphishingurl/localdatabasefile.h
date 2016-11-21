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

#ifndef LOCALDATABASEFILE_H
#define LOCALDATABASEFILE_H

#include <QString>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QtEndian>

namespace WebEngineViewer
{
class LocalDataBaseFile
{
public:
    LocalDataBaseFile(const QString &filename);
    ~LocalDataBaseFile();

    bool isValid() const { return mValid; }
    inline quint16 getUint16(int offset) const
    {
        return qFromBigEndian(*reinterpret_cast<quint16 *>(mData + offset));
    }
    inline quint32 getUint32(int offset) const
    {
        return qFromBigEndian(*reinterpret_cast<quint32 *>(mData + offset));
    }
    inline const char *getCharStar(int offset) const
    {
        return reinterpret_cast<const char *>(mData + offset);
    }


private:
    bool load();
    bool reload();

    QFile mFile;
    uchar *mData;
    QDateTime mMtime;
    bool mValid;
};
}

#endif // LOCALDATABASEFILE_H
