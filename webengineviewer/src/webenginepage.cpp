/*
   SPDX-FileCopyrightText: 2016-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webenginepage.h"
#include "webhittest.h"

#include <KLocalizedString>

#include <QEventLoop>
#include <QFileDialog>
#include <QPointer>
#include <QPrinter>
#include <QTimer>
#include <QWebEngineDownloadRequest>
#include <QWebEngineProfile>
#include <QWebEngineView>

using namespace WebEngineViewer;

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(new QWebEngineProfile, parent)
{
    // Create a private (off the record) QWebEngineProfile here to isolate the
    // browsing settings, and adopt it as a child so that it will be deleted
    // when we are destroyed.  The profile must remain active for as long as
    // any QWebEnginePage's belonging to it exist, see the API documentation
    // of QWebEnginePage::QWebEnginePage(QWebEngineProfile *, QObject *).
    // Deleting it as our child on destruction is safe.
    //
    // Do not try to save a line of code by setting the parent on construction:
    //
    //    WebEnginePage::WebEnginePage(QObject *parent)
    //      : QWebEnginePage(new QWebEngineProfile(this), parent)
    //
    // because the QWebEngineProfile constructor will call out to the QWebEnginePage
    // and crash because the QWebEnginePage is not fully constructed yet.
    profile()->setParent(this);

    init();
}

WebEnginePage::WebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
{
    init();
}

void WebEnginePage::init()
{
    connect(profile(), &QWebEngineProfile::downloadRequested, this, &WebEnginePage::saveHtml);
}

WebEngineViewer::WebHitTest *WebEnginePage::hitTestContent(const QPoint &pos)
{
    return new WebHitTest(this, mapToViewport(pos), pos);
}

QPoint WebEnginePage::mapToViewport(const QPoint &pos) const
{
    return QPoint(pos.x() / zoomFactor(), pos.y() / zoomFactor());
}

void WebEnginePage::saveHtml(QWebEngineDownloadRequest *download)
{
    const QString fileName = QFileDialog::getSaveFileName(QWebEngineView::forPage(this), i18n("Save HTML Page"));
    if (!fileName.isEmpty()) {
        download->setSavePageFormat(QWebEngineDownloadRequest::SingleHtmlSaveFormat);
        download->setDownloadDirectory(QFileInfo(fileName).path());
        download->setDownloadFileName(QFileInfo(fileName).fileName());
        download->accept();
    }
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
    if (isMainFrame && type == NavigationTypeLinkClicked) {
        Q_EMIT urlClicked(url);
        return false;
    }
    return true;
}

void WebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                             const QString &message,
                                             int lineNumber,
                                             const QString &sourceID)
{
    Q_UNUSED(level)
    Q_UNUSED(sourceID)
    // Don't convert to debug categories
    qDebug() << "WebEnginePage::javaScriptConsoleMessage lineNumber: " << lineNumber << " message: " << message;
    Q_EMIT showConsoleMessage(message);
}
