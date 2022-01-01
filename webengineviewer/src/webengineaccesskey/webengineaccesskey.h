/*
   SPDX-FileCopyrightText: 2016-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <memory>

class KActionCollection;
class QWheelEvent;
class QResizeEvent;
class QKeyEvent;
class QWebEngineView;
namespace WebEngineViewer
{
class WebEngineAccessKeyPrivate;
/**
 * @brief The WebEngineAccessKey class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineAccessKey : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineAccessKey(QWebEngineView *webEngine, QObject *parent = nullptr);
    ~WebEngineAccessKey() override;

    void setActionCollection(KActionCollection *ac);

    void wheelEvent(QWheelEvent *e);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void showAccessKeys();

Q_SIGNALS:
    void openUrl(const QUrl &url);
public Q_SLOTS:
    void hideAccessKeys();

private Q_SLOTS:
    void handleSearchAccessKey(const QVariant &res);

private:
    std::unique_ptr<WebEngineAccessKeyPrivate> const d;
};
}

