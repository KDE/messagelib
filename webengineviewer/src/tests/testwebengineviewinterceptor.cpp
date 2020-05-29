/*
   Copyright (C) 2020 Laurent Montel <montel@kde.org>

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

#include "testwebengineviewinterceptor.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QBuffer>

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
    qDebug() << " void CidSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)";
    //TODO
}

class CidUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
public:
    explicit CidUrlRequestInterceptor(QObject *p = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;
    ~CidUrlRequestInterceptor();
};

CidUrlRequestInterceptor::CidUrlRequestInterceptor(QObject *p)
    : QWebEngineUrlRequestInterceptor(p)
{

}
#define LOAD_FROM_FILE 1
void CidUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QUrl urlRequestUrl(info.requestUrl());
    if (urlRequestUrl.scheme() == QLatin1String("cid")) {
        qDebug() << " info.resourceType() "<< info.resourceType();
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage) {
            QUrl r;
            if (urlRequestUrl.url() == QLatin1String("cid:resource_src")) {
                qDebug() << " from resource src";
                r = QUrl(QStringLiteral("qrc:audio-volume-medium.png"));
            } else if (urlRequestUrl.url() == QLatin1String("cid:local_src")) {
                qDebug() << " from local file";
#ifdef LOAD_FROM_FILE
                QImage image(QLatin1String(PICSRC "/audio-volume-medium.png"));
                QByteArray ba;
                QBuffer buf(&ba);
                image.save(&buf, "png");
                const QByteArray hexed = ba.toBase64();
                buf.close();
                r = QUrl(QString::fromUtf8(QByteArray("data:image/png;base64,") + hexed));
#else
                r = QUrl::fromLocalFile(QLatin1String(PICSRC "/audio-volume-medium.png"));
#endif
            }
            qDebug() << "urlRequestUrl  " << urlRequestUrl;
            qDebug() << " r " << r;
            info.redirect(r);
        }
    } else if (urlRequestUrl.scheme() == QLatin1String("file")) {
        //info.block(true);
    }
}

CidUrlRequestInterceptor::~CidUrlRequestInterceptor()
{

}

TestWebEngineViewInterceptor::TestWebEngineViewInterceptor(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    mWebEngineView = new QWebEngineView(this);
    mWebEngineView->page()->profile()->setUrlRequestInterceptor(new CidUrlRequestInterceptor(this));

    const QString htmlStr = QStringLiteral("<html>"
                                           "<head>"
                                           "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
                                           "</head>"
                                           "<body>"
                                           "<p>image from resource file<img src=\"cid:resource_src\" /> end</p>"
                                           "<p>image from local file<img src=\"cid:local_src\" /> end</p>"
                                           "<p>Xcv</p>"
                                           "<p>bxcvbxcvbxcvb</p>"
                                           "<br /></body>"
                                           "</html>");

    mWebEngineView->setHtml(htmlStr/*, QUrl(QStringLiteral("file:///"))*/);
    vbox->addWidget(mWebEngineView);
}

int main(int argc, char *argv[])
{
    QWebEngineUrlScheme scheme("cid");
    scheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    scheme.setFlags(QWebEngineUrlScheme::LocalScheme|QWebEngineUrlScheme::LocalAccessAllowed);
    QWebEngineUrlScheme::registerScheme(scheme);

    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    CidSchemeHandler *handler = new CidSchemeHandler(nullptr);
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("cid", handler);
    TestWebEngineViewInterceptor *testWebEngine = new TestWebEngineViewInterceptor;
    testWebEngine->show();
    testWebEngine->resize(600, 400);
    const int ret = app.exec();
    return ret;
}
