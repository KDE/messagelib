/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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

#ifndef WEBENGINEMANAGESCRIPT_H
#define WEBENGINEMANAGESCRIPT_H

#include <QObject>
#include <QWebEngineScript>
#include "webengineviewer_export.h"

class QWebEngineProfile;
namespace WebEngineViewer {
class WEBENGINEVIEWER_EXPORT WebEngineManageScript : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineManageScript(QObject *parent = nullptr);
    ~WebEngineManageScript();
    void addScript(QWebEngineProfile *profile, const QString &source, const QString &scriptName, QWebEngineScript::InjectionPoint injectionPoint);

    static quint32 scriptWordId()
    {
        return QWebEngineScript::UserWorld + 1;
    }
};
}
#endif // WEBENGINEMANAGESCRIPT_H
