/*
  Copyright (c) 2016-2017 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/
#include "scamdetectionwebengine.h"
#include "scamcheckshorturlmanager.h"
#include "scamdetectiondetailsdialog.h"
#include "settings/messageviewersettings.h"
#include "MessageViewer/ScamCheckShortUrl"
#include "webengineviewer/webenginescript.h"
#include <WebEngineViewer/WebEngineManageScript>

#include <KLocalizedString>

#include <QRegularExpression>
#include <QPointer>
#include <QWebEnginePage>

using namespace MessageViewer;

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    QPointer<R> receiver;
    void (C::*memberFunction)(Arg);
    void operator()(Arg result)
    {
        if (receiver) {
            (receiver->*memberFunction)(result);
        }
    }
};

template<typename Arg, typename R, typename C>

InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFunction)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFunction};
    return wrapper;
}

static QString addWarningColor(const QString &url)
{
    const QString error = QStringLiteral("<font color=#FF0000>%1</font>").arg(url);
    return error;
}

class MessageViewer::ScamDetectionWebEnginePrivate
{
public:
    ScamDetectionWebEnginePrivate()
    {

    }
    QString mDetails;
    QPointer<MessageViewer::ScamDetectionDetailsDialog> mDetailsDialog;
};

ScamDetectionWebEngine::ScamDetectionWebEngine(QObject *parent)
    : QObject(parent),
      d(new MessageViewer::ScamDetectionWebEnginePrivate)
{

}

ScamDetectionWebEngine::~ScamDetectionWebEngine()
{
    delete d;
}

void ScamDetectionWebEngine::scanPage(QWebEnginePage *page)
{
    if (MessageViewer::MessageViewerSettings::self()->scamDetectionEnabled()) {
        page->runJavaScript(WebEngineViewer::WebEngineScript::findAllAnchorsAndForms(),
                            WebEngineViewer::WebEngineManageScript::scriptWordId(),
                            invoke(this, &ScamDetectionWebEngine::handleScanPage));
    }
}

void ScamDetectionWebEngine::handleScanPage(const QVariant &result)
{
    bool foundScam = false;

    d->mDetails.clear();
    const QVariantList resultList = result.toList();
    if (resultList.count() != 1) {
        Q_EMIT resultScanDetection(foundScam);
        return;
    }
    d->mDetails = QLatin1String("<b>") + i18n("Details:") + QLatin1String("</b><ul>");
    QRegularExpression ip4regExp(QStringLiteral("\\b[0-9]{1,3}\\.[0-9]{1,3}(?:\\.[0-9]{0,3})?(?:\\.[0-9]{0,3})?"));
    const QVariantMap mapResult = resultList.at(0).toMap();
    const QList<QVariant> lst = mapResult.value(QStringLiteral("anchors")).toList();
    for (const QVariant &var : lst) {
        QMap<QString, QVariant> mapVariant = var.toMap();
        //qDebug()<<" mapVariant"<<mapVariant;

        //1) detect if title has a url and title != href
        const QString title = mapVariant.value(QStringLiteral("title")).toString();
        const QString href = mapVariant.value(QStringLiteral("src")).toString();
        const QUrl url(href);
        if (!title.isEmpty()) {
            if (title.startsWith(QStringLiteral("http:"))
                    || title.startsWith(QStringLiteral("https:"))
                    || title.startsWith(QStringLiteral("www."))) {
                if (title.startsWith(QStringLiteral("www."))) {
                    const QString completUrl =  url.scheme() + QLatin1String("://") + title;
                    if (completUrl != href &&
                            href != (completUrl + QLatin1Char('/'))) {
                        foundScam = true;
                    }
                } else {
                    if (href != title) {
                        // http://www.kde.org == http://www.kde.org/
                        if (href != (title + QLatin1Char('/'))) {
                            foundScam = true;
                        }
                    }
                }
                if (foundScam) {
                    d->mDetails += QLatin1String("<li>") + i18n("This email contains a link which reads as '%1' in the text, but actually points to '%2'. This is often the case in scam emails to mislead the recipient", addWarningColor(title), addWarningColor(href)) + QLatin1String("</li>");
                }
            }
        }
        if (!foundScam) {
            //2) detect if url href has ip and not server name.
            const QString hostname = url.host();
            if (hostname.contains(ip4regExp) && !hostname.contains(QStringLiteral("127.0.0.1"))) { //hostname
                d->mDetails += QLatin1String("<li>") + i18n("This email contains a link which points to a numerical IP address (%1) instead of a typical textual website address. This is often the case in scam emails.", addWarningColor(hostname)) + QLatin1String("</li>");
                foundScam = true;
            } else if (hostname.contains(QLatin1Char('%'))) { //Hexa value for ip
                d->mDetails += QLatin1String("<li>") + i18n("This email contains a link which points to a hexadecimal IP address (%1) instead of a typical textual website address. This is often the case in scam emails.", addWarningColor(hostname)) + QLatin1String("</li>");
                foundScam = true;
            } else if (url.toString().contains(QStringLiteral("url?q="))) { //4) redirect url.
                d->mDetails += QLatin1String("<li>") + i18n("This email contains a link (%1) which has a redirection", addWarningColor(url.toString())) + QLatin1String("</li>");
                foundScam = true;
            } else if ((url.toString().count(QStringLiteral("http://")) > 1) ||
                       (url.toString().count(QStringLiteral("https://")) > 1)) { //5) more that 1 http in url.
                if (!url.toString().contains(QStringLiteral("kmail:showAuditLog"))) {
                    d->mDetails += QLatin1String("<li>") + i18n("This email contains a link (%1) which contains multiple http://. This is often the case in scam emails.", addWarningColor(url.toString())) + QLatin1String("</li>");
                    foundScam = true;
                }
            }
        }
        //Check shortUrl
        if (!foundScam) {
            if (ScamCheckShortUrl::isShortUrl(url)) {
                d->mDetails += QLatin1String("<li>") + i18n("This email contains a shorturl (%1). It can redirect to another server.", addWarningColor(url.toString())) + QLatin1String("</li>");
                foundScam = true;
            }
        }
    }
    if (mapResult.value(QStringLiteral("forms")).toInt() > 0) {
        d->mDetails += QLatin1String("<li></b>") + i18n("Message contains form element. This is often the case in scam emails.") + QLatin1String("</b></li>");
        foundScam = true;
    }
    d->mDetails += QLatin1String("</ul>");
    //qDebug()<<" d->mDetails "<< d->mDetails;
    if (foundScam) {
        Q_EMIT messageMayBeAScam();
    }
    Q_EMIT resultScanDetection(foundScam);
}

void ScamDetectionWebEngine::showDetails()
{
    if (!d->mDetailsDialog) {
        d->mDetailsDialog = new MessageViewer::ScamDetectionDetailsDialog;
    }
    d->mDetailsDialog->setDetails(d->mDetails);
    d->mDetailsDialog->show();
}
