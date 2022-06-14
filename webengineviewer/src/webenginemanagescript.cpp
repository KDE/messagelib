/*
   SPDX-FileCopyrightText: 2016-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webenginemanagescript.h"
#include "webengineviewer_debug.h"

#include <QWebEngineProfile>
#include <QWebEngineScriptCollection>

using namespace WebEngineViewer;

WebEngineManageScript::WebEngineManageScript(QObject *parent)
    : QObject(parent)
{
}

WebEngineManageScript::~WebEngineManageScript() = default;
