/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <QWebEngineScript>

class QWebEngineProfile;
namespace WebEngineViewer
{
/**
 * @brief The WebEngineManageScript class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineManageScript : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineManageScript(QObject *parent = nullptr);
    ~WebEngineManageScript() override;
    void addScript(QWebEngineProfile *profile, const QString &source, const QString &scriptName, QWebEngineScript::InjectionPoint injectionPoint);

    static quint32 scriptWordId()
    {
        return QWebEngineScript::UserWorld + 1;
    }
};
}
