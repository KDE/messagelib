/*
  Copyright (c) 2016-2020 Laurent Montel <montel@kde.org>

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
#include <QTemporaryFile>
#include <QDir>

using namespace MessageViewer;

WebEnginePartHtmlWriter::WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent)
    : QObject(parent)
    , mHtmlView(view)
{
    assert(view);
}

WebEnginePartHtmlWriter::~WebEnginePartHtmlWriter()
{
    delete mTempFile;
}

void WebEnginePartHtmlWriter::begin()
{
    if (mState != Ended) {
        qCWarning(MESSAGEVIEWER_LOG) << "begin() called on non-ended session!";
        reset();
    }

    delete mTempFile;

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
    //mHtmlView->setContent(data(), QStringLiteral("text/html;charset=UTF-8"), QUrl(QStringLiteral("file:///")));

    mTempFile = new QTemporaryFile(QDir::tempPath() + QLatin1String("/messageviewer_XXXXXX")+ QLatin1String(".html"));
    mTempFile->open();
    QTextStream stream(mTempFile);
    //TODO we need to change codec when mail use different codec.
    QByteArray codecValue = codec();
    if (codecValue.isEmpty()) {
        codecValue = QByteArray("UTF-8");
    }
    //qDebug() << " codecValue ******************************************: " << codecValue;
    if (data().contains("<meta charset=\"utf-8\">")) {
        codecValue = QByteArray("UTF-8");
    }
    stream.setCodec(codecValue.constData());
    stream << data();

    //Bug 387061
    mHtmlView->load(QUrl::fromLocalFile(mTempFile->fileName()));
    //qDebug() << " tempFile.fileName()" << mTempFile->fileName();
    mHtmlView->show();
    mTempFile->close();
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
