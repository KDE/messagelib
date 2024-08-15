/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "webengineparthtmlwriter.h"
#include "webengineembedpart.h"

#include "messageviewer/messageviewerutil.h"
#include "messageviewer_debug.h"
#include "viewer/webengine/mailwebengineview.h"
#include <QUrl>

#include <QDir>
#include <QTemporaryFile>
#include <cassert>

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
    mTempFile = nullptr;

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
    if (!mStyleBody.isEmpty()) {
        insertBodyStyle();
        mStyleBody.clear();
    }
    if (data().size() > 2000000) {
        // qDebug() << " load big message ";
        mTempFile = new QTemporaryFile(QDir::tempPath() + QLatin1StringView("/messageviewer_XXXXXX") + QLatin1StringView(".html"));
        mTempFile->open();
        mTempFile->write(data());
        // Bug 387061
        mHtmlView->load(QUrl::fromLocalFile(mTempFile->fileName()));
        // qDebug() << " tempFile.fileName()" << mTempFile->fileName();
        mHtmlView->show();
        mTempFile->close();
    } else {
        mHtmlView->setContent(data(), QStringLiteral("text/html;charset=UTF-8"), QUrl(QStringLiteral("file:///")));
        mHtmlView->show();
    }
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

void WebEnginePartHtmlWriter::insertBodyStyle()
{
    const auto bodyTag(QByteArrayLiteral("<body>"));
    const int index = m_data.indexOf(bodyTag);
    if (index != -1) {
        m_data.insert(index + bodyTag.length() - 1, mStyleBody.toUtf8());
    }
}

void WebEnginePartHtmlWriter::setStyleBody(const QString &styleBody)
{
    mStyleBody = styleBody;
}

void WebEnginePartHtmlWriter::setExtraHead(const QString &str)
{
    mExtraHead = str;
}

#include "moc_webengineparthtmlwriter.cpp"
