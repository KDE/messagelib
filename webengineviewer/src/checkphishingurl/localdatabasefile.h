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

#include "webengineviewer_export.h"
#include <QString>
#include <QDateTime>
#include <QFile>

namespace WebEngineViewer
{
class LocalDataBaseFilePrivate;
class WEBENGINEVIEWER_EXPORT LocalDataBaseFile
{
public:
    LocalDataBaseFile(const QString &filename);
    ~LocalDataBaseFile();

    bool reload();

    bool isValid() const;
    inline quint16 getUint16(int offset) const;
    inline quint32 getUint32(int offset) const;
    inline const char *getCharStar(int offset) const;

    QString searchHash(int posListOffset, const QByteArray &hashToSearch);

    bool shouldCheck();
private:
    LocalDataBaseFilePrivate *const d;
};
}

#endif // LOCALDATABASEFILE_H
