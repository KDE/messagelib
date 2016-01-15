/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "webpage.h"
#include "networkaccessmanager.h"
#include "adblockmanager.h"

using namespace MessageViewer;

class MessageViewer::WebPagePrivate
{
public:
    WebPagePrivate()
    {

    }
    QUrl mLoadingUrl;
    MessageViewer::MyNetworkAccessManager *mCustomNetworkManager;
};

WebPage::WebPage(QWidget *parent)
    : KWebPage(parent),
      d(new WebPagePrivate)
{
    d->mCustomNetworkManager = new MessageViewer::MyNetworkAccessManager(this);
    d->mCustomNetworkManager->setEmitReadyReadOnMetaDataChange(true);
    d->mCustomNetworkManager->setCache(0);
    QWidget *window = parent ? parent->window() : 0;
    if (window) {
        d->mCustomNetworkManager->setWindow(window);
    }
    setNetworkAccessManager(d->mCustomNetworkManager);
    connect(this, SIGNAL(frameCreated(QWebFrame*)), AdBlockManager::self(), SLOT(applyHidingRules(QWebFrame*)));
}

WebPage::~WebPage()
{
    delete d;
}

QUrl WebPage::loadingUrl() const
{
    return d->mLoadingUrl;
}

void WebPage::setDoNotTrack(bool state)
{
    d->mCustomNetworkManager->setDoNotTrack(state);
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
    const bool isMainFrameRequest = (frame == mainFrame());
    if (isMainFrameRequest) {
        d->mLoadingUrl = request.url();
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}
