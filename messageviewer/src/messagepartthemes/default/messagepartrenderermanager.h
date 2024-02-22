/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <QObject>

#include <KTextTemplate/Template>

#include <QMetaType>

#include <functional>

namespace GrantleeTheme
{
class Engine;
}

namespace MessageViewer
{
class GlobalContext;

using KTextTemplateCallback = std::function<void(KTextTemplate::OutputStream *)>;
/**
 * @brief The MessagePartRendererManager class
 */
class MESSAGEVIEWER_EXPORT MessagePartRendererManager : public QObject
{
    Q_OBJECT
public:
    ~MessagePartRendererManager() override;
    static MessagePartRendererManager *self();
    [[nodiscard]] KTextTemplate::Template loadByName(const QString &name);
    [[nodiscard]] KTextTemplate::Context createContext();

private:
    explicit MessagePartRendererManager(QObject *parent = nullptr);
    MESSAGEVIEWER_NO_EXPORT void initializeRenderer();
    GrantleeTheme::Engine *m_engine = nullptr;
    GlobalContext *const m_globalContext;
};
}

Q_DECLARE_METATYPE(MessageViewer::KTextTemplateCallback)
