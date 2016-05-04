/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

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

#include "cidreferencesurlinterceptor.h"
#include "htmlwriter/webengineembedpart.h"

#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;

CidReferencesUrlInterceptor::CidReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{

}

CidReferencesUrlInterceptor::~CidReferencesUrlInterceptor()
{
}

bool CidReferencesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    if (info.requestUrl().scheme() == QLatin1String("cid")) {
        const QString newUrl = MessageViewer::WebEngineEmbedPart::self()->contentUrl(info.requestUrl().path());
        if (!newUrl.isEmpty()) {
            info.redirect(QUrl(newUrl));
        }
    }
    return false;
}
