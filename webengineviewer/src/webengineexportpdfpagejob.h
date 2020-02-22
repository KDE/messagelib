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

#ifndef WEBENGINEEXPORTPDFPAGEJOB_H
#define WEBENGINEEXPORTPDFPAGEJOB_H

#include <QObject>
#include "webengineviewer_export.h"
class QWebEngineView;
namespace WebEngineViewer {
/**
 * @brief The WebEngineExportHtmlPageJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineExportPdfPageJob : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineExportPdfPageJob(QObject *parent = nullptr);
    ~WebEngineExportPdfPageJob();
    void start();

    Q_REQUIRED_RESULT QWebEngineView *engineView() const;
    void setEngineView(QWebEngineView *engineView);

    Q_REQUIRED_RESULT QString pdfPath() const;
    void setPdfPath(const QString &pdfPath);

    Q_REQUIRED_RESULT bool canStart() const;

private:
    QString mPdfPath;
    QWebEngineView *mWebEngineView = nullptr;
};
}

#endif // WEBENGINEEXPORTPDFPAGEJOB_H
