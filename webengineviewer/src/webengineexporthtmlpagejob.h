/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QWebEngineView *engineView() const;
    void setEngineView(QWebEngineView *engineView);

Q_SIGNALS:
    void failed();
    void success(const QString &filename);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotSaveHtmlToPage(const QString &text);
    QWebEngineView *mEngineView = nullptr;
};
}
