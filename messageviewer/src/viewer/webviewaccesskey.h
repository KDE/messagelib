/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef WEBVIEWACCESSKEY_H
#define WEBVIEWACCESSKEY_H

#include <QObject>
#include "messageviewer_export.h"
class QKeyEvent;
class QWebElement;
class QWebView;
class KActionCollection;
class QWheelEvent;
class QResizeEvent;
namespace MessageViewer
{
class WebViewAccessKeyPrivate;
class MESSAGEVIEWER_EXPORT WebViewAccessKey : public QObject
{
    Q_OBJECT
public:
    enum AccessKeyState {
        NotActivated,
        PreActivated,
        Activated
    };

    explicit WebViewAccessKey(QWebView *webView, QObject *parent = Q_NULLPTR);
    ~WebViewAccessKey();

    void setActionCollection(KActionCollection *ac);


    bool checkForAccessKey(QKeyEvent *event);
    void showAccessKeys();
    void makeAccessKeyLabel(QChar accessKey, const QWebElement &element);

    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *);
public Q_SLOTS:
    void hideAccessKeys();

private:
    WebViewAccessKeyPrivate *const d;
};
}
#endif // WEBVIEWACCESSKEY_H
