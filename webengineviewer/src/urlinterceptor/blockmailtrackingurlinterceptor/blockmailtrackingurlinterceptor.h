/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QVector>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace WebEngineViewer
{
/**
 * @brief The BlockMailTrackingUrlInterceptor class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT BlockMailTrackingUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    struct MailTrackerBlackList {
        MailTrackerBlackList() = default;
        MailTrackerBlackList(const QString &company, const QString &pattern, const QString &url)
            : mCompanyName(company)
            , mCompanyUrl(url)
            , mPattern(pattern)
        {
        }

        QString mCompanyName;
        QString mCompanyUrl;
        QString mPattern;
    };

    explicit BlockMailTrackingUrlInterceptor(QObject *parent = nullptr);
    ~BlockMailTrackingUrlInterceptor() override;
    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;

    Q_REQUIRED_RESULT bool enabledMailTrackingInterceptor() const;
    void setEnabledMailTrackingInterceptor(bool enabledMailTrackingInterceptor);

Q_SIGNALS:
    void trackingFound(const WebEngineViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);

private:
    void initializeList();
    QVector<MailTrackerBlackList> mBackList;
    bool mEnabledMailTrackingInterceptor = false;
};
}
Q_DECLARE_TYPEINFO(WebEngineViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(WebEngineViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList)