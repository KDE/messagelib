/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <QUrl>
#include <memory>

namespace WebEngineViewer
{
class CheckPhishingUrlCachePrivate;
/**
 * @brief The CheckPhishingUrlCache class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlCache : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlCache(QObject *parent = nullptr);
    static CheckPhishingUrlCache *self();

    enum UrlStatus {
        UrlOk = 0,
        MalWare = 1,
        Unknown = 2,
    };

    ~CheckPhishingUrlCache() override;

    /**
     * @brief addCheckingUrlResult cache url. If @p correctUrl is true we store as UrlOk otherwise MalWare
     * @param url
     * @param correctUrl
     */
    void addCheckingUrlResult(const QUrl &url, bool correctUrl, uint cacheDuration = 0);
    /**
     * @brief urlStatus Return the status of cached Url. When we didn't stored it, it returns Unknown
     * @param url
     * @return the status of url
     */
    [[nodiscard]] CheckPhishingUrlCache::UrlStatus urlStatus(const QUrl &url);

    /**
     * @brief clearCache clear the cache and save result in config file.
     */
    void clearCache();

private:
    std::unique_ptr<CheckPhishingUrlCachePrivate> const d;
};
}

Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlCache::UrlStatus)
