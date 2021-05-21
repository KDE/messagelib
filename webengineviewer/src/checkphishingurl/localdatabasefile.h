/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_private_export.h"
#include <QString>

namespace WebEngineViewer
{
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
    Q_REQUIRED_RESULT bool fileExists() const;

    bool reload();

    Q_REQUIRED_RESULT bool isValid() const;
    Q_REQUIRED_RESULT quint16 getUint16(int offset) const;
    Q_REQUIRED_RESULT quint32 getUint32(int offset) const;
    Q_REQUIRED_RESULT quint64 getUint64(int offset) const;
    const char *getCharStar(int offset) const;

    Q_REQUIRED_RESULT QByteArray searchHash(const QByteArray &hashToSearch);

    Q_REQUIRED_RESULT bool shouldCheck() const;
    Q_REQUIRED_RESULT bool checkFileChanged();
    Q_REQUIRED_RESULT QVector<WebEngineViewer::Addition> extractAllInfo() const;

private:
    LocalDataBaseFilePrivate *const d;
};
}

