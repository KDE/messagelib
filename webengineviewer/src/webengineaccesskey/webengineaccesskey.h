/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#ifndef MAILWEBENGINEACCESSKEY_H
#define MAILWEBENGINEACCESSKEY_H

#include "webengineviewer_export.h"
#include <QObject>

class KActionCollection;
class QWheelEvent;
class QResizeEvent;
class QKeyEvent;
class QWebEngineView;
namespace WebEngineViewer {
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
    ~WebEngineAccessKey();

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
    WebEngineAccessKeyPrivate *const d;
};
}

#endif
