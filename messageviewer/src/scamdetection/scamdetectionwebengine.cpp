/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#include "scamdetectionwebengine.h"
#include "MessageViewer/ScamCheckShortUrl"
#include "scamdetectiondetailsdialog.h"
#include "settings/messageviewersettings.h"
#include "webengineviewer/webenginescript.h"
#include <WebEngineViewer/WebEngineManageScript>

#include <KLocalizedString>

#include <QPointer>
#include <QRegularExpression>
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
    ScamDetectionWebEnginePrivate() = default;

    QString mDetails;
    QPointer<MessageViewer::ScamDetectionDetailsDialog> mDetailsDialog;
};

ScamDetectionWebEngine::ScamDetectionWebEngine(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::ScamDetectionWebEnginePrivate)
{
}

ScamDetectionWebEngine::~ScamDetectionWebEngine() = default;

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
    static const QRegularExpression ip4regExp(QStringLiteral("\\b[0-9]{1,3}\\.[0-9]{1,3}(?:\\.[0-9]{0,3})?(?:\\.[0-9]{0,3})?"));
    const QVariantMap mapResult = resultList.at(0).toMap();
    const QList<QVariant> lst = mapResult.value(QStringLiteral("anchors")).toList();
    for (const QVariant &var : lst) {
        QMap<QString, QVariant> mapVariant = var.toMap();
        // qDebug()<<" mapVariant"<<mapVariant;

        // 1) detect if title has a url and title != href
        const QString title = mapVariant.value(QStringLiteral("title")).toString();
        QString href = mapVariant.value(QStringLiteral("src")).toString();
        if (!QUrl(href).toString().contains(QLatin1StringView("kmail:showAuditLog"))) {
            href = href.toLower();
        }
        const QUrl url(href);
        if (!title.isEmpty()) {
            if (title.startsWith(QLatin1StringView("http:")) || title.startsWith(QLatin1StringView("https:")) || title.startsWith(QLatin1StringView("www."))) {
                if (title.startsWith(QLatin1StringView("www."))) {
                    const QString completUrl = url.scheme() + QLatin1StringView("://") + title;
                    if (completUrl != href && href != (completUrl + QLatin1Char('/'))) {
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
                    d->mDetails += QLatin1StringView("<li>")
                        + i18n("This email contains a link which reads as '%1' in the text, but actually points to '%2'. This is often the case in scam emails "
                               "to mislead the recipient",
                               addWarningColor(title),
                               addWarningColor(href))
                        + QLatin1StringView("</li>");
                }
            }
        }
        if (!foundScam) {
            // 2) detect if url href has ip and not server name.
            const QString hostname = url.host();
            if (hostname.contains(ip4regExp) && !hostname.contains(QLatin1StringView("127.0.0.1"))) { // hostname
                d->mDetails += QLatin1StringView("<li>")
                    + i18n("This email contains a link which points to a numerical IP address (%1) instead of a typical textual website address. This is often "
                           "the case in scam emails.",
                           addWarningColor(hostname))
                    + QLatin1StringView("</li>");
                foundScam = true;
            } else if (hostname.contains(QLatin1Char('%'))) { // Hexa value for ip
                d->mDetails += QLatin1StringView("<li>")
                    + i18n("This email contains a link which points to a hexadecimal IP address (%1) instead of a typical textual website address. This is "
                           "often the case in scam emails.",
                           addWarningColor(hostname))
                    + QLatin1StringView("</li>");
                foundScam = true;
            } else if (url.toString().contains(QLatin1StringView("url?q="))) { // 4) redirect url.
                d->mDetails += QLatin1StringView("<li>") + i18n("This email contains a link (%1) which has a redirection", addWarningColor(url.toString()))
                    + QLatin1StringView("</li>");
                foundScam = true;
            } else if ((url.toString().count(QStringLiteral("http://")) > 1)
                       || (url.toString().count(QStringLiteral("https://")) > 1)) { // 5) more that 1 http in url.
                if (!url.toString().contains(QLatin1StringView("kmail:showAuditLog"))) {
                    d->mDetails += QLatin1StringView("<li>")
                        + i18n("This email contains a link (%1) which contains multiple http://. This is often the case in scam emails.",
                               addWarningColor(url.toString()))
                        + QLatin1StringView("</li>");
                    foundScam = true;
                }
            }
        }
        // Check shortUrl
        if (!foundScam) {
            if (ScamCheckShortUrl::isShortUrl(url)) {
                d->mDetails += QLatin1StringView("<li>")
                    + i18n("This email contains a shorturl (%1). It can redirect to another server.", addWarningColor(url.toString()))
                    + QLatin1StringView("</li>");
                foundScam = true;
            }
        }
        if (!foundScam) {
            QUrl displayUrl = QUrl(mapVariant.value(QStringLiteral("text")).toString());
            // Special case if https + port 443 it will return url without port
            QString text = (displayUrl.port() == 443 && displayUrl.scheme() == QLatin1StringView("https"))
                ? displayUrl.toDisplayString(QUrl::StripTrailingSlash | QUrl::NormalizePathSegments | QUrl::RemovePort)
                : displayUrl.toDisplayString(QUrl::StripTrailingSlash | QUrl::NormalizePathSegments);
            if (text.endsWith(QLatin1StringView("%22"))) {
                text.chop(3);
            }
            const QUrl normalizedHrefUrl = QUrl(href.toLower());
            QString normalizedHref = normalizedHrefUrl.toDisplayString(QUrl::StripTrailingSlash | QUrl::NormalizePathSegments);
            if (text != normalizedHref) {
                if (normalizedHref.contains(QStringLiteral("%5C"))) {
                    normalizedHref.replace(QStringLiteral("%5C"), QStringLiteral("/"));
                }
            }
            if (normalizedHref.endsWith(QLatin1StringView("%22"))) {
                normalizedHref.chop(3);
            }
            // qDebug() << "text " << text << " href "<<href << " normalizedHref " << normalizedHref;

            if (!text.isEmpty()) {
                if (text.startsWith(QLatin1StringView("http:/")) || text.startsWith(QLatin1StringView("https:/"))) {
                    if (text.toLower() != normalizedHref.toLower()) {
                        if (text != normalizedHref) {
                            if (normalizedHref != (text + QLatin1Char('/'))) {
                                if (normalizedHref.toHtmlEscaped() != text) {
                                    if (QString::fromUtf8(QUrl(text).toEncoded()) != normalizedHref) {
                                        if (QUrl(normalizedHref).toDisplayString() != text) {
                                            const bool qurlqueryequal = displayUrl.query() == normalizedHrefUrl.query();
                                            const QString displayUrlWithoutQuery =
                                                displayUrl.toDisplayString(QUrl::RemoveQuery | QUrl::StripTrailingSlash | QUrl::NormalizePathSegments);
                                            const QString hrefUrlWithoutQuery =
                                                normalizedHrefUrl.toDisplayString(QUrl::RemoveQuery | QUrl::StripTrailingSlash | QUrl::NormalizePathSegments);
                                            // qDebug() << "displayUrlWithoutQuery "  << displayUrlWithoutQuery << " hrefUrlWithoutQuery " <<
                                            // hrefUrlWithoutQuery << " text " << text;
                                            if (qurlqueryequal && (displayUrlWithoutQuery + QLatin1Char('/') != hrefUrlWithoutQuery)) {
                                                d->mDetails += QLatin1StringView("<li>")
                                                    + i18n("This email contains a link which reads as '%1' in the text, but actually points to '%2'. This is "
                                                           "often "
                                                           "the case in scam emails to mislead the recipient",
                                                           addWarningColor(text),
                                                           addWarningColor(normalizedHref))
                                                    + QLatin1StringView("</li>");
                                                foundScam = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (mapResult.value(QStringLiteral("forms")).toInt() > 0) {
        d->mDetails +=
            QLatin1StringView("<li></b>") + i18n("Message contains form element. This is often the case in scam emails.") + QLatin1StringView("</b></li>");
        foundScam = true;
    }
    if (foundScam) {
        d->mDetails.prepend(QLatin1StringView("<b>") + i18n("Details:") + QLatin1StringView("</b><ul>"));
        d->mDetails += QLatin1StringView("</ul>");
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

#include "moc_scamdetectionwebengine.cpp"
