/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <grantlee/template.h>

#include <QMetaType>
#include <QObject>

#include <functional>

namespace GrantleeTheme
{
class Engine;
}

namespace MessageViewer
{
class GlobalContext;

using GrantleeCallback = std::function<void(Grantlee::OutputStream *)>;
/**
 * @brief The MessagePartRendererManager class
 */
class MESSAGEVIEWER_EXPORT MessagePartRendererManager : public QObject
{
    Q_OBJECT
public:
    explicit MessagePartRendererManager(QObject *parent = nullptr);
    ~MessagePartRendererManager() override;
    static MessagePartRendererManager *self();

    Q_REQUIRED_RESULT Grantlee::Template loadByName(const QString &name);
    Q_REQUIRED_RESULT Grantlee::Context createContext();

private:
    void initializeRenderer();
    GrantleeTheme::Engine *m_engine = nullptr;
    GlobalContext *m_globalContext;
};
}

Q_DECLARE_METATYPE(MessageViewer::GrantleeCallback)

