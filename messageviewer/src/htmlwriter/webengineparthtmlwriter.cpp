/*
  Copyright (c) 2016-2018 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "webengineparthtmlwriter.h"
#include "webengineembedpart.h"

#include "messageviewer_debug.h"
#include "viewer/webengine/mailwebengineview.h"

#include <QUrl>

#include <cassert>
#include <QByteArray>

using namespace MessageViewer;

WebEnginePartHtmlWriter::WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent)
    : QObject(parent)
    , mHtmlView(view)
    , mState(Ended)
{
    assert(view);
}

WebEnginePartHtmlWriter::~WebEnginePartHtmlWriter()
{
}

void WebEnginePartHtmlWriter::begin()
{
    if (mState != Ended) {
        qCWarning(MESSAGEVIEWER_LOG) << "begin() called on non-ended session!";
        reset();
    }

    BufferedHtmlWriter::begin();
    MessageViewer::WebEngineEmbedPart::self()->clear();
    mState = Begun;
}

void WebEnginePartHtmlWriter::end()
{
    BufferedHtmlWriter::end();
    if (mState != Begun) {
        qCWarning(MESSAGEVIEWER_LOG) << "Called on non-begun or queued session!";
    }
    if (!mExtraHead.isEmpty()) {
        insertExtraHead();
        mExtraHead.clear();
    }
    // see QWebEnginePage::setHtml()
    mHtmlView->setContent(data(), QStringLiteral("text/html;charset=UTF-8"), QUrl(QStringLiteral("file:///")));
    mHtmlView->show();
    clear();

    mHtmlView->setUpdatesEnabled(true);
    mHtmlView->update();
    mState = Ended;
    Q_EMIT finished();
}

void WebEnginePartHtmlWriter::reset()
{
    BufferedHtmlWriter::reset();
    if (mState != Ended) {
        mState = Begun; // don't run into end()'s warning
        end();
        mState = Ended;
    }
}

void WebEnginePartHtmlWriter::embedPart(const QByteArray &contentId, const QString &contentURL)
{
    MessageViewer::WebEngineEmbedPart::self()->addEmbedPart(contentId, contentURL);
}

void WebEnginePartHtmlWriter::insertExtraHead()
{
    const auto headTag(QByteArrayLiteral("<head>"));
    const int index = m_data.indexOf(headTag);
    if (index != -1) {
        m_data.insert(index + headTag.length(), mExtraHead.toUtf8());
    }
}

void WebEnginePartHtmlWriter::extraHead(const QString &str)
{
    mExtraHead = str;
}
