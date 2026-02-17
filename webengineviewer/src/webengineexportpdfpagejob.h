/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
class QWebEngineView;
namespace WebEngineViewer
{
/*!
 * \class WebEngineViewer::WebEngineExportPdfPageJob
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/WebEngineExportPdfPageJob
 *
 * \brief The WebEngineExportHtmlPageJob class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineExportPdfPageJob : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a WebEngineExportPdfPageJob with the given parent. */
    explicit WebEngineExportPdfPageJob(QObject *parent = nullptr);
    /*! Destroys the WebEngineExportPdfPageJob object. */
    ~WebEngineExportPdfPageJob() override;
    /*! Starts the PDF export job. */
    void start();

    /*! Returns the QWebEngineView associated with this job. */
    [[nodiscard]] QWebEngineView *engineView() const;
    /*! Sets the QWebEngineView for the PDF export. */
    void setEngineView(QWebEngineView *engineView);

    /*! Returns the PDF file path. */
    [[nodiscard]] QString pdfPath() const;
    /*! Sets the PDF file path. */
    void setPdfPath(const QString &pdfPath);

    /*! Returns whether the job can be started. */
    [[nodiscard]] bool canStart() const;

Q_SIGNALS:
    /*! Emitted when the PDF export fails. */
    void exportPdfFailed();
    /*! Emitted when the PDF export succeeds. */
    void exportToPdfSuccess();

private:
    WEBENGINEVIEWER_NO_EXPORT void slotExportPdfFinished(const QString &filePath, bool success);
    QString mPdfPath;
    QWebEngineView *mWebEngineView = nullptr;
};
}
