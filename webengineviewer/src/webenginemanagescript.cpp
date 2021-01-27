/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webenginemanagescript.h"
#include "webengineviewer_debug.h"

#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

using namespace WebEngineViewer;

WebEngineManageScript::WebEngineManageScript(QObject *parent)
    : QObject(parent)
{
}

WebEngineManageScript::~WebEngineManageScript()
{
}

void WebEngineManageScript::addScript(QWebEngineProfile *profile,
                                      const QString &source,
                                      const QString &scriptName,
                                      QWebEngineScript::InjectionPoint injectionPoint)
{
    if (profile) {
        QWebEngineScript script;
        const QList<QWebEngineScript> collectionScripts = profile->scripts()->findScripts(scriptName);
        if (!collectionScripts.isEmpty()) {
            script = collectionScripts.first();
        }
        for (const QWebEngineScript &s : collectionScripts) {
            profile->scripts()->remove(s);
        }

        if (script.isNull()) {
            script.setName(scriptName);
            script.setInjectionPoint(injectionPoint);
            script.setRunsOnSubFrames(true);
            script.setWorldId(scriptWordId());
        }
        script.setSourceCode(source);
        profile->scripts()->insert(script);
        // qCDebug(WEBENGINEVIEWER_LOG) << " void WebEngineManageScript::addScript profile:" << profile;
    }
}
