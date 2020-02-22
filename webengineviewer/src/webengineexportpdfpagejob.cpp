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
