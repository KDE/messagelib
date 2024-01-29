/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testwebengineviewinterceptor.h"
#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineView>

class CidSchemeHandler : public QWebEngineUrlSchemeHandler
{
public:
    explicit CidSchemeHandler(QObject *parent = nullptr);
    void requestStarted(QWebEngineUrlRequestJob *request) override;
};

CidSchemeHandler::CidSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

void CidSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    Q_UNUSED(request)
    qDebug() << " void CidSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)";
    // TODO
}

class CidUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
public:
    explicit CidUrlRequestInterceptor(QObject *p = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
    ~CidUrlRequestInterceptor() override;
};

CidUrlRequestInterceptor::CidUrlRequestInterceptor(QObject *p)
    : QWebEngineUrlRequestInterceptor(p)
{
}

#define LOAD_FROM_FILE 1
void CidUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QUrl urlRequestUrl(info.requestUrl());
    qDebug() << " urlRequestUrl " << urlRequestUrl;
    if (urlRequestUrl.scheme() == QLatin1StringView("cid")) {
        qDebug() << " info.resourceType() " << info.resourceType();
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage) {
            QUrl r;
            if (urlRequestUrl.url() == QLatin1StringView("cid:resource_src")) {
                qDebug() << " from resource src";
                r = QUrl(QStringLiteral("qrc:audio-volume-medium.png"));
            } else if (urlRequestUrl.url() == QLatin1StringView("cid:local_src")) {
                qDebug() << " from local file";
#ifdef LOAD_FROM_FILE
                QImage image(QStringLiteral(PICSRC "/audio-volume-medium.png"));
                QByteArray ba;
                QBuffer buf(&ba);
                image.save(&buf, "png");
                const QByteArray hexed = ba.toBase64();
                buf.close();
                r = QUrl(QString::fromUtf8(QByteArray("data:image/png;base64,") + hexed));
#else
                r = QUrl::fromLocalFile(QLatin1StringView(PICSRC "/audio-volume-medium.png"));
#endif
            }
            qDebug() << "urlRequestUrl  " << urlRequestUrl;
            qDebug() << " r " << r;
            info.redirect(r);
        }
    } else if (urlRequestUrl.scheme() == QLatin1StringView("file")) {
        // info.block(true);
    }
}

CidUrlRequestInterceptor::~CidUrlRequestInterceptor() = default;

TestWebEngineViewInterceptor::TestWebEngineViewInterceptor(QWidget *parent)
    : QWidget(parent)
{
    auto vbox = new QVBoxLayout(this);
    mWebEngineView = new QWebEngineView(this);
    mWebEngineView->page()->profile()->setUrlRequestInterceptor(new CidUrlRequestInterceptor(this));

    const QString htmlStr = QStringLiteral(
        "<html>"
        "<head>"
        "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
        "</head>"
        "<body>"
        "<img alt=\"GitLab\" class=\"footer-logo\" "
        "src=\"https://invent.kde.org/assets/mailers/gitlab_logo_black_text-5430ca955baf2bbce6d3aa856a025da70ac5c9595597537254f665c10beab7a5.png\" "
        "style=\"display: block; width: 90px; margin: 0 auto 1em;\">"
        "<p>image from resource file <img src=\"cid:resource_src\" /> end</p>"
        "<p>image from local file <img src=\"cid:local_src\" /> end</p>"
        "<p>Xcv</p>"
        "<p>bxcvbxcvbxcvb</p>"
        "<br /></body>"
        "</html>");

    mWebEngineView->setHtml(htmlStr /*, QUrl(QStringLiteral("file:///"))*/);
    vbox->addWidget(mWebEngineView);
}

int main(int argc, char *argv[])
{
    QWebEngineUrlScheme cidScheme("cid");
    cidScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::LocalScheme
                       | QWebEngineUrlScheme::LocalAccessAllowed);
    cidScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    QWebEngineUrlScheme::registerScheme(cidScheme);

    QApplication app(argc, argv);
    auto handler = new CidSchemeHandler(nullptr);
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("cid", handler);
    auto testWebEngine = new TestWebEngineViewInterceptor;
    testWebEngine->show();
    testWebEngine->resize(600, 400);
    const int ret = app.exec();
    return ret;
}

#include "moc_testwebengineviewinterceptor.cpp"
