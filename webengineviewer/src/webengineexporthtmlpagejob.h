/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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

