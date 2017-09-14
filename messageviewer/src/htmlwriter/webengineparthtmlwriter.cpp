/*
  Copyright (c) 2016-2017 Montel Laurent <montel@kde.org>

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
#include "webengineembedpart.h"

#include "messageviewer_debug.h"
#include "viewer/webengine/mailwebengineview.h"

#include <QUrl>

#include <cassert>
#include <QByteArray>

using namespace MessageViewer;

WebEnginePartHtmlWriter::WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent)
    : QObject(parent)
    , MimeTreeParser::HtmlWriter()
    , mHtmlView(view)
    , mState(Ended)
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

    MessageViewer::WebEngineEmbedPart::self()->clear();
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

void WebEnginePartHtmlWriter::embedPart(const QByteArray &contentId, const QString &contentURL)
{
    MessageViewer::WebEngineEmbedPart::self()->addEmbedPart(contentId, contentURL);
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
