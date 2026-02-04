/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::HashCacheManager
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/HashCacheManager
 *
 * \brief The HashCacheManager class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT HashCacheManager : public QObject
{
    Q_OBJECT
public:
    enum UrlStatus : uint8_t {
        UrlOk = 0,
        MalWare = 1,
        Unknown = 2,
    };
    explicit HashCacheManager(QObject *parent = nullptr);
    /*! Destroys the HashCacheManager object. */
    ~HashCacheManager() override;

    /*! Returns the singleton instance. */
    static HashCacheManager *self();

    /*! Clears the cache. */
    void clearCache();

    /*! Adds a hash status to the cache. */
    void addHashStatus(const QByteArray &hash, HashCacheManager::UrlStatus status, uint cacheDuration);

    /*! Returns the status of a hash. */
    [[nodiscard]] HashCacheManager::UrlStatus hashStatus(const QByteArray &hash);

private:
    std::unique_ptr<HashCacheManagerPrivate> const d;
};
}

Q_DECLARE_METATYPE(WebEngineViewer::HashCacheManager::UrlStatus)
