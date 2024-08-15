/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_private_export.h"
#include <QString>
#include <memory>

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
    [[nodiscard]] bool fileExists() const;

    [[nodiscard]] bool reload();

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] quint16 getUint16(int offset) const;
    [[nodiscard]] quint32 getUint32(int offset) const;
    [[nodiscard]] quint64 getUint64(int offset) const;
    [[nodiscard]] const char *getCharStar(int offset) const;

    [[nodiscard]] QByteArray searchHash(const QByteArray &hashToSearch);

    [[nodiscard]] bool shouldCheck() const;
    [[nodiscard]] bool checkFileChanged();
    [[nodiscard]] QList<WebEngineViewer::Addition> extractAllInfo() const;

private:
    std::unique_ptr<LocalDataBaseFilePrivate> const d;
};
}
