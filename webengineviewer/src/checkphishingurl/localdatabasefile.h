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
class Addition;
class WEBENGINEVIEWER_EXPORT LocalDataBaseFile
{
public:
    /*
     * binary file:
     * index 0 => quint16 => major version
     * index 2 => quint16 => minor version
     * index 4 => quint64 => number of element
     *
     * After : index of item in binary file
     *
     * value
     */
    LocalDataBaseFile(const QString &filename);
    ~LocalDataBaseFile();

    void close();
    bool fileExists() const;

    bool reload();

    bool isValid() const;
    quint16 getUint16(int offset) const;
    quint32 getUint32(int offset) const;
    quint64 getUint64(int offset) const;
    const char *getCharStar(int offset) const;

    QString searchHash(int posListOffset, const QByteArray &hashToSearch);

    bool shouldCheck();
    bool checkFileChanged();
    QVector<WebEngineViewer::Addition> extractAllInfo() const;

private:
    LocalDataBaseFilePrivate *const d;
};
}

#endif // LOCALDATABASEFILE_H
