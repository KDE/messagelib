/*
   Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "blockmailtrackingurlinterceptor.h"

#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;
BlockMailTrackingUrlInterceptor::BlockMailTrackingUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
    qRegisterMetaType<MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList>();
    initializeList();
}

BlockMailTrackingUrlInterceptor::~BlockMailTrackingUrlInterceptor()
{
}

bool BlockMailTrackingUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QUrl urlRequestUrl(info.requestUrl());
    for (int i = 0; i < mBackList.size(); ++i) {
        if (urlRequestUrl.url().startsWith(mBackList.at(i).mCompanyUrl)) {
            Q_EMIT mailTrackingFound(mBackList.at(i));
            return true;
        }
    }
    return false;
}

void BlockMailTrackingUrlInterceptor::initializeList()
{
    mBackList = {
        {QStringLiteral("Sidekick"), QStringLiteral("t.signaux"), QStringLiteral("http://getsidekick.com") },
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
        {QStringLiteral("Outreach"), QStringLiteral("app.outreach.io"), QStringLiteral("http://outreach.io")}
    };
}
