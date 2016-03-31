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
#include "webengineparthtmlwriter.h"

#include "messageviewer_debug.h"
#include "viewer/webengine/mailwebengineview.h"

#include <QUrl>

#include <cassert>
#include <QByteArray>

using namespace MessageViewer;

WebEnginePartHtmlWriter::WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent)
    : QObject(parent), MimeTreeParser::HtmlWriter(),
      mHtmlView(view), mState(Ended)
{
    assert(view);
}

WebEnginePartHtmlWriter::~WebEnginePartHtmlWriter()
{
}

void WebEnginePartHtmlWriter::begin(const QString &css)
{
    // The stylesheet is now included CSSHelper::htmlHead()
    Q_UNUSED(css);
    if (mState != Ended) {
        qCWarning(MESSAGEVIEWER_LOG) << "begin() called on non-ended session!";
        reset();
    }

    mEmbeddedPartMap.clear();
    // clear the widget:
    mHtmlView->setUpdatesEnabled(false);
    mHtmlView->scrollUp(10);
    // PENDING(marc) push into MailWebView
    mHtmlView->load(QUrl());
    mState = Begun;
}

void WebEnginePartHtmlWriter::end()
{
    if (mState != Begun) {
        qCWarning(MESSAGEVIEWER_LOG) << "Called on non-begun or queued session!";
    }
    if (!mExtraHead.isEmpty()) {
        insertExtraHead();
        mExtraHead.clear();
    }
    mHtmlView->setHtml(mHtml, QUrl(QStringLiteral("file:///")));
    mHtmlView->show();
    mHtml.clear();

    resolveCidUrls();
    mHtmlView->setUpdatesEnabled(true);
    mHtmlView->update();
    mState = Ended;
    Q_EMIT finished();
}

void WebEnginePartHtmlWriter::reset()
{
    if (mState != Ended) {
        mHtml.clear();
        mState = Begun; // don't run into end()'s warning
        end();
        mState = Ended;
    }
}

void WebEnginePartHtmlWriter::write(const QString &str)
{
    if (mState != Begun) {
        qCWarning(MESSAGEVIEWER_LOG) << "Called in Ended or Queued state!";
    }
    mHtml.append(str);
}

void WebEnginePartHtmlWriter::queue(const QString &str)
{
    write(str);
}

void WebEnginePartHtmlWriter::flush()
{
    mState = Begun; // don't run into end()'s warning
    end();
}

void WebEnginePartHtmlWriter::embedPart(const QByteArray &contentId,
                                        const QString &contentURL)
{
    mEmbeddedPartMap[QLatin1String(contentId)] = contentURL;
}

void WebEnginePartHtmlWriter::resolveCidUrls()
{
#if 0
    // FIXME: instead of patching around in the HTML source, this should
    // be replaced by a custom QNetworkAccessManager (for QWebView), or
    // virtual loadResource() (for QTextBrowser)
    QWebElement root = mHtmlView->page()->mainFrame()->documentElement();
    QWebElementCollection images = root.findAll(QStringLiteral("img"));
    QWebElementCollection::iterator end(images.end());
    for (QWebElementCollection::iterator it = images.begin(); it != end; ++it) {
        QUrl url((*it).attribute(QStringLiteral("src")));
        if (url.scheme() == QLatin1String("cid")) {
            EmbeddedPartMap::const_iterator cit = mEmbeddedPartMap.constFind(url.path());
            if (cit != mEmbeddedPartMap.constEnd()) {
                qCDebug(MESSAGEVIEWER_LOG) << "Replacing" << url.toDisplayString() << "by" << cit.value();
                (*it).setAttribute(QStringLiteral("src"), cit.value());
            }
        }
    }
#else
    qDebug() << "WebEnginePartHtmlWriter::resolveCidUrls() not implemented";
#endif
}

void WebEnginePartHtmlWriter::insertExtraHead()
{
    const QString headTag(QStringLiteral("<head>"));
    const int index = mHtml.indexOf(headTag);
    if (index != -1) {
        mHtml.insert(index + headTag.length(), mExtraHead);
    }
}

void WebEnginePartHtmlWriter::extraHead(const QString &str)
{
    mExtraHead = str;
}
