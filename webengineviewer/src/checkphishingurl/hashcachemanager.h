/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <memory>

namespace WebEngineViewer
{
class HashCacheManagerPrivate;
// https://developers.google.com/safe-browsing/v4/caching
/**
 * @brief The HashCacheManager class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT HashCacheManager : public QObject
{
    Q_OBJECT
public:
    enum UrlStatus {
        UrlOk = 0,
        MalWare = 1,
        Unknown = 2,
    };
    explicit HashCacheManager(QObject *parent = nullptr);
    ~HashCacheManager() override;

    static HashCacheManager *self();

    void clearCache();

    void addHashStatus(const QByteArray &hash, HashCacheManager::UrlStatus status, uint cacheDuration);

    [[nodiscard]] HashCacheManager::UrlStatus hashStatus(const QByteArray &hash);

private:
    std::unique_ptr<HashCacheManagerPrivate> const d;
};
}

Q_DECLARE_METATYPE(WebEngineViewer::HashCacheManager::UrlStatus)
