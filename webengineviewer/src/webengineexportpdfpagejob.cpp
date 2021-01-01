/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineexportpdfpagejob.h"
#include "webengineviewer_debug.h"
#include <QWebEngineView>

using namespace WebEngineViewer;
WebEngineExportPdfPageJob::WebEngineExportPdfPageJob(QObject *parent)
    : QObject(parent)
{
}

WebEngineExportPdfPageJob::~WebEngineExportPdfPageJob()
{
}

void WebEngineExportPdfPageJob::start()
{
    if (!canStart()) {
        qCWarning(WEBENGINEVIEWER_LOG) << "webengineview not defined or path is not defined.! It's a bug";
        Q_EMIT exportPdfFailed();
        deleteLater();
        return;
    }
    connect(mWebEngineView->page(), &QWebEnginePage::pdfPrintingFinished, this, &WebEngineExportPdfPageJob::slotPdfPrintingFinished);
    mWebEngineView->page()->printToPdf(mPdfPath);
}

void WebEngineExportPdfPageJob::slotPdfPrintingFinished(const QString &filePath, bool success)
{
    if (success) {
        Q_EMIT exportToPdfSuccess();
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Failed to export to pdf to " << filePath;
        Q_EMIT exportPdfFailed();
    }
    deleteLater();
}

QWebEngineView *WebEngineExportPdfPageJob::engineView() const
{
    return mWebEngineView;
}

void WebEngineExportPdfPageJob::setEngineView(QWebEngineView *engineView)
{
    mWebEngineView = engineView;
}

QString WebEngineExportPdfPageJob::pdfPath() const
{
    return mPdfPath;
}

void WebEngineExportPdfPageJob::setPdfPath(const QString &pdfPath)
{
    mPdfPath = pdfPath;
}

bool WebEngineExportPdfPageJob::canStart() const
{
    return mWebEngineView && !mPdfPath.isEmpty();
}
