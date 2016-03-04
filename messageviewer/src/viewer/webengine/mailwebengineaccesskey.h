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

#ifndef MAILWEBENGINEACCESSKEY_H
#define MAILWEBENGINEACCESSKEY_H

#include "messageviewer_export.h"
#include <QObject>

class KActionCollection;
class QWheelEvent;
class QResizeEvent;
class QKeyEvent;
namespace MessageViewer
{
class MailWebEngineAccessKeyPrivate;
class MESSAGEVIEWER_EXPORT MailWebEngineAccessKey : public QObject
{
    Q_OBJECT
public:
    explicit MailWebEngineAccessKey(QObject *parent = Q_NULLPTR);
    ~MailWebEngineAccessKey();

    void setActionCollection(KActionCollection *ac);

    void wheelEvent(QWheelEvent *e);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void showAccessKeys();
    //void makeAccessKeyLabel(QChar accessKey, const QWebElement &element);

    bool checkForAccessKey(QKeyEvent *event);
public Q_SLOTS:
    void hideAccessKeys();

private:
    MailWebEngineAccessKeyPrivate *const d;
};
}

#endif
