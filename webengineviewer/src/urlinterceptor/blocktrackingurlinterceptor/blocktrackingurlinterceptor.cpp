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
    const QString url = urlRequestUrl.url();
    for (const auto &blackListinfo : std::as_const(mBackList)) {
        if (blackListinfo.mPattern.match(url).hasMatch()) {
            qCDebug(WEBENGINEVIEWER_BLOCK_TRACKING_URL_LOG) << " found tracker " << blackListinfo;
            Q_EMIT trackingFound(blackListinfo);
            return true;
        }
    }
    return false;
}

void BlockTrackingUrlInterceptor::initializeList()
{
    // Patterns are regular expressions matched against the whole request url.
    // Sources: https://github.com/OneClickLab/ugly-email-trackers
    //          https://github.com/JannikArndt/EMailTrackerBlocker
    mBackList = {
        {u"ActiveCampaign"_s, uR"(\/lt\.php.*\?l=open)"_s, u"https://www.activecampaign.com"_s},
        {u"Amazon SES"_s, uR"(\/\w*I0\/\w{16}-\w{8}-\w{4})"_s, u"https://aws.amazon.com/ses/"_s},
        {u"Apple"_s, uR"(apple\.com\/report\/2\/its_mail_sf)"_s, u"https://www.apple.com"_s},
        {u"AWeber"_s, uR"(openrate\.aweber\.com)"_s, u"https://www.aweber.com"_s},
        {u"Banana Tag"_s, uR"(bl-1\.com)"_s, u"https://bananatag.com"_s},
        {u"Boomerang"_s, uR"(mailstat\.us\/tr)"_s, u"https://www.boomeranggmail.com"_s},
        {u"Campaign Monitor"_s, uR"(cmail1\.com\/t)"_s, u"https://www.campaignmonitor.com"_s},
        {u"Cirrus Insight"_s, uR"(tracking\.cirrusinsight\.com)"_s, u"https://www.cirrusinsight.com"_s},
        {u"Close"_s, uR"(close\.io\/email_opened|\/\/ml\.closeml\.com\/t\/\w+\/\w+\.png)"_s, u"https://close.com"_s},
        {u"Constant Contact"_s, uR"(\.net\/on\.jsp\?)"_s, u"https://www.constantcontact.com"_s},
        {u"ContactMonkey"_s, uR"(contactmonkey\.com\/api\/v1\/tracker)"_s, u"https://www.contactmonkey.com"_s},
        {u"ConvertKit"_s, uR"(\/\/.*\.convertkit-mail[1-9]?\.com\/o\/)"_s, u"https://convertkit.com"_s},
        {u"Cooper Press"_s, uR"(\/open\/\d{3}\/\w{10})"_s, u"https://cooperpress.com"_s},
        {u"Drip"_s, uR"(dripemail2)"_s, u"https://www.drip.com"_s},
        {u"FreshMail"_s, uR"(\/o\/\w{10}\/\w{10}\/)"_s, u"https://freshmail.com"_s},
        {u"Front App"_s, uR"(web\.frontapp\.com\/api)"_s, u"https://frontapp.com"_s},
        {u"GetNotify"_s, uR"(email81\.com\/case)"_s, u"https://www.getnotify.com"_s},
        {u"GetResponse"_s, uR"(\/open\.html\?x=)"_s, u"https://www.getresponse.com"_s},
        {u"GrowthDot"_s, uR"(growthdot\.com\/api\/mail-tracking)"_s, u"https://www.growthdot.com"_s},
        {u"HubSpot"_s,
         uR"(t\.(hubspotemail|hubspotfree|hubspotstarter|signaux|senal|signale|sidekickopen|sigopn|hsms|strk)|track\.getsidekick\.com|\/e2t\/(o|c|to)\/)"_s,
         u"https://www.hubspot.com"_s},
        {u"iContact"_s, uR"(click\.icptrack\.com\/icp\/)"_s, u"https://www.icontact.com"_s},
        {u"Infusionsoft"_s, uR"(infusionsoft\.com\/app\/emailOpened)"_s, u"https://keap.com"_s},
        {u"Intercom"_s, uR"(via\.intercom\.io\/o|\/\/.*\.intercom-\w+\.com(\/via)?\/)"_s, u"https://www.intercom.com"_s},
        {u"LaunchBit"_s, uR"(launchbit\.com\/taz-pixel)"_s, u"https://launchbit.com"_s},
        {u"Litmus"_s, uR"(emltrk\.com)"_s, u"https://www.litmus.com"_s},
        {u"MailChimp"_s, uR"(list-manage\.com\/track|\/track\/open\.php\?u=)"_s, u"https://mailchimp.com"_s},
        {u"Mailgun"_s, uR"(\/e\/o\/[A-Za-z0-9+\/=]+)"_s, u"https://www.mailgun.com"_s},
        {u"Mailjet"_s, uR"(\/\/links\..*\/oo\/|\.mjt\.lu\/|\/oo\/.*\.gif)"_s, u"https://www.mailjet.com"_s},
        {u"Mailspring"_s, uR"(getmailspring\.com\/open)"_s, u"https://getmailspring.com"_s},
        {u"Mailtrack.io"_s, uR"(mailtrack\.io\/trace|mltrk\.io\/pixel\/)"_s, u"https://mailtrack.io"_s},
        {u"Mandrill"_s, uR"(mandrillapp\.com\/track)"_s, u"https://mandrillapp.com"_s},
        {u"Marketo"_s, uR"(\/trk\?t=)"_s, u"https://www.marketo.com"_s},
        {u"MixMax"_s, uR"((email|track)\.mixmax\.com|mixmax\.com\/(api\/track|e\/o)\/)"_s, u"https://www.mixmax.com"_s},
        {u"Mixpanel"_s, uR"(mixpanel\.com\/(trk|track))"_s, u"https://mixpanel.com"_s},
        {u"NetHunt"_s, uR"(nethunt\.com\/api\/v1\/track\/email\/)"_s, u"https://nethunt.com"_s},
        {u"Outreach"_s, uR"(app\.outreach\.io|(outrch|whosen|getoutreach)\.com\/api\/mailings\/opened)"_s, u"https://www.outreach.io"_s},
        {u"PersistIQ"_s, uR"(infinite-stream-5194\.herokuapp\.com\/pixel\/)"_s, u"https://www.persistiq.com"_s},
        {u"phpList"_s, uR"(\/ut\.php\?u=)"_s, u"https://www.phplist.com"_s},
        {u"Polymail"_s, uR"(polymail\.io(\/v2\/z\/)?|share\.polymail\.io)"_s, u"https://polymail.io"_s},
        {u"Postmark"_s, uR"(pstmrk\.it\/open)"_s, u"https://postmarkapp.com"_s},
        {u"RelateIQ"_s, uR"(app\.relateiq\.com\/t\.png)"_s, u"https://www.salesforce.com"_s},
        {u"Return Path"_s, uR"(returnpath\.net\/pixel\.gif)"_s, u"https://www.validity.com"_s},
        {u"RJ Metrics"_s, uR"(go\.rjmetrics\.com)"_s, u"https://magento.com"_s},
        {u"Sailthru"_s, uR"(sailthru\.com\/trk)"_s, u"https://www.sailthru.com"_s},
        {u"Salesforce"_s, uR"(\/\/click\..*\/open\.aspx|salesforceiq\.com\/t\.png|beacon\.krxd\.net)"_s, u"https://www.salesforce.com"_s},
        {u"SalesHandy"_s, uR"(saleshandy\.com\/web\/email\/countopened)"_s, u"https://www.saleshandy.com"_s},
        {u"SalesLoft"_s, uR"(salesloft\.com\/email_trackers|salesloftlinks\.com\/t\/)"_s, u"https://salesloft.com"_s},
        {u"Sare"_s, uR"(\/\/.*\.enewsletter\.pl\/.*\.gif)"_s, u"https://www.sare.pl"_s},
        {u"Segment"_s, uR"(email\.segment\.com\/e\/o\/)"_s, u"https://segment.com"_s},
        {u"Selligent"_s, uR"(\/optiext\/optiextension\.dll\?ID=)"_s, u"https://www.selligent.com"_s},
        {u"SendGrid"_s, uR"(\/wf\/open\?upn=)"_s, u"https://sendgrid.com"_s},
        {u"Sendinblue"_s, uR"(sendib\w{2}\.com\/track\/)"_s, u"https://www.brevo.com"_s},
        {u"Shopify"_s, uR"(\/tools\/emails\/open\/)"_s, u"https://www.shopify.com"_s},
        {u"SparkPost"_s, uR"(go\.sparkpostmail2\.com\/q\/)"_s, u"https://www.sparkpost.com"_s},
        {u"Streak"_s, uR"(mailfoogae\.appspot\.com)"_s, u"https://www.streak.com"_s},
        {u"Superhuman"_s, uR"(r\.superhuman\.com)"_s, u"https://superhuman.com"_s},
        {u"Technolutions"_s, uR"(\/\/mx\.technolutions\.net\/ss\/o\/.*\/ho\.gif)"_s, u"https://technolutions.com"_s},
        {u"TinyLetter"_s, uR"(tinyletterapp\.com)"_s, u"https://tinyletter.com"_s},
        {u"ToutApp"_s, uR"(toutapp\.com)"_s, u"https://www.marketo.com"_s},
        {u"TrackApp"_s, uR"(trackapp\.io\/(static\/img\/track\.gif|[a-z]\/))"_s, u"https://trackapp.io"_s},
        {u"Vocus"_s, uR"(\/\/tracking\.vocus\.io\/)"_s, u"https://vocus.io"_s},
        {u"YAMM"_s, uR"(yamm-track\.appspot)"_s, u"https://yet-another-mail-merge.com"_s},
        {u"Yesware"_s, uR"((app|t)\.yesware\.com)"_s, u"https://www.yesware.com"_s}};
    for (auto &blackListinfo : mBackList) {
        blackListinfo.mPattern.optimize();
    }
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
    d << "mPattern : " << t.mPattern.pattern();
    return d;
}

#include "moc_blocktrackingurlinterceptor.cpp"
