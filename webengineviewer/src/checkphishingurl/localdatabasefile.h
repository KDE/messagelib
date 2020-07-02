/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LOCALDATABASEFILE_H
#define LOCALDATABASEFILE_H

#include "webengineviewer_private_export.h"
#include <QString>

namespace WebEngineViewer {
class LocalDataBaseFilePrivate;
struct Addition;
class WEBENGINEVIEWER_TESTS_EXPORT LocalDataBaseFile
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
    explicit LocalDataBaseFile(const QString &filename);
    ~LocalDataBaseFile();

    void close();
    bool fileExists() const;

    bool reload();

    bool isValid() const;
    quint16 getUint16(int offset) const;
    quint32 getUint32(int offset) const;
    quint64 getUint64(int offset) const;
    const char *getCharStar(int offset) const;

    QByteArray searchHash(const QByteArray &hashToSearch);

    bool shouldCheck() const;
    bool checkFileChanged();
    QVector<WebEngineViewer::Addition> extractAllInfo() const;

private:
    LocalDataBaseFilePrivate *const d;
};
}

#endif // LOCALDATABASEFILE_H
