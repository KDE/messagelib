/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWebEngineScript>

class QWebEngineProfile;
namespace WebEngineViewer
{
/**
 * @brief The WebEngineManageScript class
 * @author Laurent Montel <montel@kde.org>
 */
struct WEBENGINEVIEWER_EXPORT WebEngineManageScript {
    static quint32 scriptWordId()
    {
        return QWebEngineScript::UserWorld + 1;
    }
};
}
