/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINEEXPORTHTMLPAGEJOB_H
#define WEBENGINEEXPORTHTMLPAGEJOB_H

#include <QObject>
#include "webengineviewer_export.h"
class QWebEngineView;
namespace WebEngineViewer {
/**
 * @brief The WebEngineExportHtmlPageJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineExportHtmlPageJob : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineExportHtmlPageJob(QObject *parent = nullptr);
    ~WebEngineExportHtmlPageJob() override;

    void start();

    Q_REQUIRED_RESULT QWebEngineView *engineView() const;
    void setEngineView(QWebEngineView *engineView);

Q_SIGNALS:
    void failed();
    void success(const QString &filename);

private Q_SLOTS:
    void slotSaveHtmlToPage(const QString &text);

private:
    QWebEngineView *mEngineView = nullptr;
};
}

#endif // WEBENGINEEXPORTHTMLPAGEJOB_H
