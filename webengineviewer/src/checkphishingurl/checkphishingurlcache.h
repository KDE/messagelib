/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \brief The CheckPhishingUrlCache class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlCache : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a CheckPhishingUrlCache object. */
    explicit CheckPhishingUrlCache(QObject *parent = nullptr);
    /*! Returns the singleton instance. */
    static CheckPhishingUrlCache *self();

    enum class UrlStatus : uint8_t {
        UrlOk = 0,
        MalWare = 1,
        Unknown = 2,
    };

    ~CheckPhishingUrlCache() override;

    /*! Adds a URL checking result to the cache. If correctUrl is true it stores as UrlOk, otherwise MalWare. */
    void addCheckingUrlResult(const QUrl &url, bool correctUrl, uint cacheDuration = 0);
    /*! Returns the status of the cached URL. When the URL is not stored, it returns Unknown. */
    [[nodiscard]] CheckPhishingUrlCache::UrlStatus urlStatus(const QUrl &url);

    /*! Clears the cache and saves result in the configuration file. */
    void clearCache();

private:
    std::unique_ptr<CheckPhishingUrlCachePrivate> const d;
};
}

Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlCache::UrlStatus)
