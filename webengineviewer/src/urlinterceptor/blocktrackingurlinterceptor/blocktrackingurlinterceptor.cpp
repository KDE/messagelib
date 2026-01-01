/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blocktrackingurlinterceptor.h"
using namespace Qt::Literals::StringLiterals;

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
    mBackList = {{u"Sidekick"_s, u"t.signaux"_s, QStringLiteral("http://getsidekick.com")},
                 {u"Sidekick"_s, u"t.sidekickopen"_s, QStringLiteral("http://getsidekick.com")},
                 {u"Sidekick"_s, u"t.sigopn"_s, QStringLiteral("http://getsidekick.com")},
                 {u"Banana Tag"_s, u"bl-1.com"_s, QStringLiteral("http://bananatag.com")},
                 {u"Boomerang"_s, u"mailstat.us/tr"_s, QStringLiteral("http://boomeranggmail.com")},
                 {u"Cirrus Insight"_s, u"tracking.cirrusinsight.com"_s, QStringLiteral("http://cirrusinsight.com")},
                 {u"Yesware"_s, u"app.yesware.com"_s, QStringLiteral("http://yesware.com")},
                 {u"Yesware"_s, u"t.yesware.com"_s, QStringLiteral("http://yesware.com")},
                 {u"Streak"_s, u"mailfoogae.appspot.com"_s, QStringLiteral("http://streak.com")},
                 {u"LaunchBit"_s, u"launchbit.com/taz-pixel"_s, QStringLiteral("http://launchbit.com")},
                 {u"MailChimp"_s, u"list-manage.com/track"_s, QStringLiteral("http://mailchimp.com")},
                 {u"Postmark"_s, u"cmail1.com/t"_s, QStringLiteral("http://postmarkapp.com")},
                 {u"iContact"_s, u"click.icptrack.com/icp/"_s, QStringLiteral("http://icontact.com")},
                 {u"Infusionsoft"_s, u"infusionsoft.com/app/emailOpened"_s, QStringLiteral("http://infusionsoft.com")},
                 {u"Intercom"_s, u"via.intercom.io/o"_s, QStringLiteral("http://intercom.io")},
                 {u"Mandrill"_s, u"mandrillapp.com/track"_s, QStringLiteral("http://mandrillapp.com")},
                 {u"Hubspot"_s, u"t.hsms06.com"_s, QStringLiteral("http://hubspot.com")},
                 {u"RelateIQ"_s, u"app.relateiq.com/t.png"_s, QStringLiteral("http://relateiq.com")},
                 {u"RJ Metrics"_s, u"go.rjmetrics.com"_s, QStringLiteral("http://rjmetrics.com")},
                 {u"Mixpanel"_s, u"api.mixpanel.com/track"_s, QStringLiteral("http://mixpanel.com")},
                 {u"Front App"_s, u"web.frontapp.com/api"_s, QStringLiteral("http://frontapp.com")},
                 {u"Mailtrack.io"_s, u"mailtrack.io/trace"_s, QStringLiteral("http://mailtrack.io")},
                 {u"ToutApp"_s, u"go.toutapp.com"_s, QStringLiteral("http://toutapp.com")},
                 {u"Outreach"_s, u"app.outreach.io"_s, QStringLiteral("http://outreach.io")}};
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
