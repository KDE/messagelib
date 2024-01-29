/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blocktrackingurlinterceptor.h"
#include "webengineviewer_block_tracking_url_interceptor_debug.h"
#include <QWebEngineUrlRequestInfo>

using namespace WebEngineViewer;
BlockTrackingUrlInterceptor::BlockTrackingUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
    qRegisterMetaType<WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList>();
    initializeList();
}

BlockTrackingUrlInterceptor::~BlockTrackingUrlInterceptor() = default;

bool BlockTrackingUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    if (!mEnabledMailTrackingInterceptor) {
        return false;
    }
    const QUrl urlRequestUrl(info.requestUrl());
    if (urlRequestUrl.scheme() != QLatin1StringView("data")) {
        qCDebug(WEBENGINEVIEWER_BLOCK_TRACKING_URL_LOG) << " Tracking url " << urlRequestUrl;
    }
    for (int i = 0, total = mBackList.size(); i < total; ++i) {
        const auto blackListinfo{mBackList.at(i)};
        if (urlRequestUrl.url().startsWith(blackListinfo.mCompanyUrl)) {
            qCDebug(WEBENGINEVIEWER_BLOCK_TRACKING_URL_LOG) << " found tracker " << blackListinfo;
            Q_EMIT trackingFound(blackListinfo);
            return true;
        }
    }
    return false;
}

void BlockTrackingUrlInterceptor::initializeList()
{
    mBackList = {{QStringLiteral("Sidekick"), QStringLiteral("t.signaux"), QStringLiteral("http://getsidekick.com")},
                 {QStringLiteral("Sidekick"), QStringLiteral("t.sidekickopen"), QStringLiteral("http://getsidekick.com")},
                 {QStringLiteral("Sidekick"), QStringLiteral("t.sigopn"), QStringLiteral("http://getsidekick.com")},
                 {QStringLiteral("Banana Tag"), QStringLiteral("bl-1.com"), QStringLiteral("http://bananatag.com")},
                 {QStringLiteral("Boomerang"), QStringLiteral("mailstat.us/tr"), QStringLiteral("http://boomeranggmail.com")},
                 {QStringLiteral("Cirrus Insight"), QStringLiteral("tracking.cirrusinsight.com"), QStringLiteral("http://cirrusinsight.com")},
                 {QStringLiteral("Yesware"), QStringLiteral("app.yesware.com"), QStringLiteral("http://yesware.com")},
                 {QStringLiteral("Yesware"), QStringLiteral("t.yesware.com"), QStringLiteral("http://yesware.com")},
                 {QStringLiteral("Streak"), QStringLiteral("mailfoogae.appspot.com"), QStringLiteral("http://streak.com")},
                 {QStringLiteral("LaunchBit"), QStringLiteral("launchbit.com/taz-pixel"), QStringLiteral("http://launchbit.com")},
                 {QStringLiteral("MailChimp"), QStringLiteral("list-manage.com/track"), QStringLiteral("http://mailchimp.com")},
                 {QStringLiteral("Postmark"), QStringLiteral("cmail1.com/t"), QStringLiteral("http://postmarkapp.com")},
                 {QStringLiteral("iContact"), QStringLiteral("click.icptrack.com/icp/"), QStringLiteral("http://icontact.com")},
                 {QStringLiteral("Infusionsoft"), QStringLiteral("infusionsoft.com/app/emailOpened"), QStringLiteral("http://infusionsoft.com")},
                 {QStringLiteral("Intercom"), QStringLiteral("via.intercom.io/o"), QStringLiteral("http://intercom.io")},
                 {QStringLiteral("Mandrill"), QStringLiteral("mandrillapp.com/track"), QStringLiteral("http://mandrillapp.com")},
                 {QStringLiteral("Hubspot"), QStringLiteral("t.hsms06.com"), QStringLiteral("http://hubspot.com")},
                 {QStringLiteral("RelateIQ"), QStringLiteral("app.relateiq.com/t.png"), QStringLiteral("http://relateiq.com")},
                 {QStringLiteral("RJ Metrics"), QStringLiteral("go.rjmetrics.com"), QStringLiteral("http://rjmetrics.com")},
                 {QStringLiteral("Mixpanel"), QStringLiteral("api.mixpanel.com/track"), QStringLiteral("http://mixpanel.com")},
                 {QStringLiteral("Front App"), QStringLiteral("web.frontapp.com/api"), QStringLiteral("http://frontapp.com")},
                 {QStringLiteral("Mailtrack.io"), QStringLiteral("mailtrack.io/trace"), QStringLiteral("http://mailtrack.io")},
                 {QStringLiteral("ToutApp"), QStringLiteral("go.toutapp.com"), QStringLiteral("http://toutapp.com")},
                 {QStringLiteral("Outreach"), QStringLiteral("app.outreach.io"), QStringLiteral("http://outreach.io")}};
    // https://github.com/JannikArndt/EMailTrackerBlocker
}

bool BlockTrackingUrlInterceptor::enabledMailTrackingInterceptor() const
{
    return mEnabledMailTrackingInterceptor;
}

void BlockTrackingUrlInterceptor::setEnabledMailTrackingInterceptor(bool enabledMailTrackingInterceptor)
{
    mEnabledMailTrackingInterceptor = enabledMailTrackingInterceptor;
}

QDebug operator<<(QDebug d, const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &t)
{
    d << "CompanyName : " << t.mCompanyName;
    d << "mCompanyUrl : " << t.mCompanyUrl;
    d << "mPattern : " << t.mPattern;
    return d;
}

#include "moc_blocktrackingurlinterceptor.cpp"
