/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
class QWebEngineView;
namespace WebEngineViewer
{
/**
 * @brief The WebEngineExportHtmlPageJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineExportPdfPageJob : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineExportPdfPageJob(QObject *parent = nullptr);
    ~WebEngineExportPdfPageJob() override;
    void start();

    Q_REQUIRED_RESULT QWebEngineView *engineView() const;
    void setEngineView(QWebEngineView *engineView);

    Q_REQUIRED_RESULT QString pdfPath() const;
    void setPdfPath(const QString &pdfPath);

    Q_REQUIRED_RESULT bool canStart() const;

Q_SIGNALS:
    void exportPdfFailed();
    void exportToPdfSuccess();

private:
    void slotPdfPrintingFinished(const QString &filePath, bool success);
    QString mPdfPath;
    QWebEngineView *mWebEngineView = nullptr;
};
}

