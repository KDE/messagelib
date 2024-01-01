/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QDebug>
#include <QList>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace WebEngineViewer
{
/**
 * @brief The BlockMailTrackingUrlInterceptor class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT BlockTrackingUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    struct TrackerBlackList {
        TrackerBlackList() = default;
        TrackerBlackList(const QString &company, const QString &pattern, const QString &url)
            : mCompanyName(company)
            , mCompanyUrl(url)
            , mPattern(pattern)
        {
        }

        QString mCompanyName;
        QString mCompanyUrl;
        QString mPattern;
    };

    explicit BlockTrackingUrlInterceptor(QObject *parent = nullptr);
    ~BlockTrackingUrlInterceptor() override;
    [[nodiscard]] bool interceptRequest(QWebEngineUrlRequestInfo &info) override;

    [[nodiscard]] bool enabledMailTrackingInterceptor() const;
    void setEnabledMailTrackingInterceptor(bool enabledMailTrackingInterceptor);

Q_SIGNALS:
    void trackingFound(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);

private:
    WEBENGINEVIEWER_NO_EXPORT void initializeList();
    QList<TrackerBlackList> mBackList;
    bool mEnabledMailTrackingInterceptor = false;
};
}
Q_DECLARE_TYPEINFO(WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList)
WEBENGINEVIEWER_EXPORT QDebug operator<<(QDebug d, const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &t);
