/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blocktrackingurlinterceptor.h"

#include "webengineviewer_block_tracking_url_interceptor_debug.h"
#include <QWebEngineUrlRequestInfo>

using namespace Qt::Literals::StringLiterals;
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
    if (urlRequestUrl.scheme() != "data"_L1) {
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
    mBackList = {{u"Sidekick"_s, u"t.signaux"_s, u"http://getsidekick.com"_s},
                 {u"Sidekick"_s, u"t.sidekickopen"_s, u"http://getsidekick.com"_s},
                 {u"Sidekick"_s, u"t.sigopn"_s, u"http://getsidekick.com"_s},
                 {u"Banana Tag"_s, u"bl-1.com"_s, u"http://bananatag.com"_s},
                 {u"Boomerang"_s, u"mailstat.us/tr"_s, u"http://boomeranggmail.com"_s},
                 {u"Cirrus Insight"_s, u"tracking.cirrusinsight.com"_s, u"http://cirrusinsight.com"_s},
                 {u"Yesware"_s, u"app.yesware.com"_s, u"http://yesware.com"_s},
                 {u"Yesware"_s, u"t.yesware.com"_s, u"http://yesware.com"_s},
                 {u"Streak"_s, u"mailfoogae.appspot.com"_s, u"http://streak.com"_s},
                 {u"LaunchBit"_s, u"launchbit.com/taz-pixel"_s, u"http://launchbit.com"_s},
                 {u"MailChimp"_s, u"list-manage.com/track"_s, u"http://mailchimp.com"_s},
                 {u"Postmark"_s, u"cmail1.com/t"_s, u"http://postmarkapp.com"_s},
                 {u"iContact"_s, u"click.icptrack.com/icp/"_s, u"http://icontact.com"_s},
                 {u"Infusionsoft"_s, u"infusionsoft.com/app/emailOpened"_s, u"http://infusionsoft.com"_s},
                 {u"Intercom"_s, u"via.intercom.io/o"_s, u"http://intercom.io"_s},
                 {u"Mandrill"_s, u"mandrillapp.com/track"_s, u"http://mandrillapp.com"_s},
                 {u"Hubspot"_s, u"t.hsms06.com"_s, u"http://hubspot.com"_s},
                 {u"RelateIQ"_s, u"app.relateiq.com/t.png"_s, u"http://relateiq.com"_s},
                 {u"RJ Metrics"_s, u"go.rjmetrics.com"_s, u"http://rjmetrics.com"_s},
                 {u"Mixpanel"_s, u"api.mixpanel.com/track"_s, u"http://mixpanel.com"_s},
                 {u"Front App"_s, u"web.frontapp.com/api"_s, u"http://frontapp.com"_s},
                 {u"Mailtrack.io"_s, u"mailtrack.io/trace"_s, u"http://mailtrack.io"_s},
                 {u"ToutApp"_s, u"go.toutapp.com"_s, u"http://toutapp.com"_s},
                 {u"Outreach"_s, u"app.outreach.io"_s, u"http://outreach.io"_s}};
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
