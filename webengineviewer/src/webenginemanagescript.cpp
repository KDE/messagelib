/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "webenginemanagescript.h"

#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QDebug>

using namespace WebEngineViewer;

WebEngineManageScript::WebEngineManageScript(QObject *parent)
    : QObject(parent)
{

}

WebEngineManageScript::~WebEngineManageScript()
{

}

void WebEngineManageScript::addScript(QWebEngineProfile *profile, const QString &source, const QString &scriptName, QWebEngineScript::InjectionPoint injectionPoint)
{
    if (profile) {
        QWebEngineScript script;
        QList<QWebEngineScript> collectionScripts = profile->scripts()->findScripts(scriptName);
        if (!collectionScripts.isEmpty()) {
            script = collectionScripts.first();
        }
        Q_FOREACH (const QWebEngineScript &s, collectionScripts) {
            profile->scripts()->remove(s);
        }

        if (script.isNull()) {
            script.setName(scriptName);
            script.setInjectionPoint(injectionPoint);
            script.setRunsOnSubFrames(true);
            script.setWorldId(QWebEngineScript::MainWorld);
        }
        script.setSourceCode(source);
        profile->scripts()->insert(script);
        qDebug()<<" insert script:::::::::::::::::::::"<<script.isNull() << "scriptName"<<scriptName ;
    }
}
